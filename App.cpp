#include <tuple>
#include <algorithm>
#include <iomanip>
#include <thread>
#include "App.hpp"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include <iostream>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif


#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

constexpr int widthOfUI = 200;

static bool mousePressed = false;
static int vxSelected = -1;
static int contrStart = -1;
constexpr float captureDistance = 0.002;

static bool validSolutionPresent = false;


void LLARPapp::mouseMovement(){

  if(!mousePressed || vxSelected < 0) return;
  int xPos, yPos;
  int buttons = SDL_GetMouseState(&xPos, &yPos);
  if(xPos > width - widthOfUI) return;
  float x = ((float) xPos / (width - widthOfUI));
  float y = 1.f - ((float) yPos / height);

  G.move(vxSelected, x, y);

}

void LLARPapp::mousePress(SDL_Event& ev){
  if(ev.button.x > width - widthOfUI) return;
  float x = ((float) ev.button.x / (width - widthOfUI));
  float y = 1.f - ((float) ev.button.y / height);

  if(ev.button.button == SDL_BUTTON_LEFT){
    //convert to unit square coordinates of the drawing window
    for(auto& v : G.getVertices()){
      auto[vx, vy] = G.getPos(v);
      if( (x-vx) * (x-vx) + (y-vy)*(y-vy) < 0.006) vxSelected = v;
    }
    mousePressed = true;
    if(vxSelected == -1) G.addVertex(x,y);

  }
  if(ev.button.button == SDL_BUTTON_RIGHT){
    //convert to unit square coordinates of the drawing window
    if(contrStart == -1){
      for(auto& v : G.getVertices()){
        auto[vx, vy] = G.getPos(v);
        if( (x-vx) * (x-vx) + (y-vy)*(y-vy) < 0.006) contrStart = v;
      }
    }else{
      for(auto& v : G.getVertices()){
        auto[vx, vy] = G.getPos(v);
        if( (x-vx) * (x-vx) + (y-vy)*(y-vy) < 0.006){
          G.contract(contrStart, v);
          contrStart = -1;
          break;
        }
      }
    }
  }
}

void LLARPapp::mouseRelease(SDL_Event& ev){
  if(ev.button.x > width - widthOfUI) return;

  //convert to unit square coordinates of the drawing window
  float x = ((float) ev.button.x / (width - widthOfUI));
  float y = 1.f - ((float) ev.button.y / height);
  mousePressed = false;
  vxSelected = -1;
}

void LLARPapp::MainWindow(){

  ImGui::SetNextWindowPos(ImVec2(width-widthOfUI, 0));
  ImGui::SetNextWindowSize({(float)widthOfUI,(float)height});


  ImGui::Begin(name.c_str(), &running, ImGuiWindowFlags_AlwaysAutoResize);


  if(ImGui::Button("Quit")) running = false;

  std::string tmp = std::to_string(G.vCount()) + " Verts";
  ImGui::Text(tmp.c_str());

  ImGui::End();


  SDL_SetRenderDrawColor(SDLRenderer, 255 , 0, 0, 255);
  SDL_SetRenderDrawBlendMode(SDLRenderer, SDL_BLENDMODE_BLEND);
  for(auto& v: G.getVertices()){
    auto [x,y] = G.getPos(v);
    SDL_Rect vx{((width - widthOfUI)*x) - 2, (height*(1.f - y)) - 2, 4, 4};
    SDL_RenderFillRect(SDLRenderer, &vx);
  }
}

LLARPapp::LLARPapp(std::string name) : name{name}{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){
    std::cerr << "SDL initialisation has failed with: " << SDL_GetError() << std::endl;
    return;
  }

    // Create window with graphics context

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, window_flags);
    SDLRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, SDLRenderer);
    ImGui_ImplSDLRenderer_Init(SDLRenderer);

    G.addVertex(0.5, 0.5);
    running = true;

}

void LLARPapp::loop(){

  while(running){
    SDL_GetWindowSize(window, &width, &height);
    SDL_Event ev;
    while (SDL_PollEvent(&ev)){
      ImGui_ImplSDL2_ProcessEvent(&ev);
      if(ev.type == SDL_QUIT) running = false;
      if(ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_CLOSE && ev.window.windowID == SDL_GetWindowID(window)) running = false;
      if(ev.type == SDL_MOUSEBUTTONDOWN) mousePress(ev);
      if(ev.type == SDL_MOUSEBUTTONUP   && ev.button.button == SDL_BUTTON_LEFT) mouseRelease(ev);
    }
    mouseMovement();



    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    SDL_SetRenderDrawColor(SDLRenderer, (Uint8)(clear_color.x * 0), (Uint8)(clear_color.y * 0), (Uint8)(clear_color.z * 0), (Uint8)(clear_color.w * 255));
    SDL_RenderClear(SDLRenderer);
    ImGui::NewFrame();

    MainWindow();



    ImGui::Render();

    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(SDLRenderer);
  }
}

LLARPapp::~LLARPapp(){
  ImGui_ImplSDLRenderer_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(SDLRenderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
