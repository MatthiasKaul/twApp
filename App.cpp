#include <tuple>
#include <algorithm>
#include <iomanip>
#include <string>
#include <cmath>
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
constexpr float captureDistance = 0.0002;

static bool validSolutionPresent = false;

static bool EdgeMode = false;
static int edgeStart = -1;

static int hoveredVx = -1;



Graph getGrid(int n){
  Graph G;
  double offset = 1. / (2+n);
  for (size_t i = 0; i < n*n; i++) {
    G.addVertex( offset*(i/n + 1), offset*(i%n + 1) );
  }

  for (size_t i = 0; i < n*n; i++) {
    if(i+n < n*n) G.addEdge(i,i+n);
    if(i+1 < n*n && (i+1) % n != 0) G.addEdge(i,i+1);
  }
  return G;
}

void toggleEdgeMode(){
  EdgeMode = !EdgeMode;
  vxSelected = -1;
  contrStart = -1;
  edgeStart = -1;
}

void LLARPapp::hovered(){
  hoveredVx = -1;
  int xPos, yPos;
  int buttons = SDL_GetMouseState(&xPos, &yPos);
  if(xPos > width - widthOfUI) return;
  float x = ((float) xPos / (width - widthOfUI));
  float y = 1.f - ((float) yPos / height);

  for(auto& v : G.getVertices()){
    auto[vx, vy] = G.getPos(v);
    if( (x-vx) * (x-vx) + (y-vy)*(y-vy) < captureDistance) {hoveredVx = v; break;}
  }
}

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

  if(ev.button.button == SDL_BUTTON_LEFT && !EdgeMode){
    //convert to unit square coordinates of the drawing window
    for(auto& v : G.getVertices()){
      auto[vx, vy] = G.getPos(v);
      if( (x-vx) * (x-vx) + (y-vy)*(y-vy) < captureDistance) vxSelected = v;
    }
    mousePressed = true;
    if(vxSelected == -1) G.addVertex(x,y);
  }

  if(ev.button.button == SDL_BUTTON_LEFT && EdgeMode){
    //convert to unit square coordinates of the drawing window
    int tmp = -1;
    for(auto& v : G.getVertices()){
      auto[vx, vy] = G.getPos(v);
      if( (x-vx) * (x-vx) + (y-vy)*(y-vy) < captureDistance) tmp = v;
    }
    if(tmp != -1){
      if(edgeStart == -1) {
        edgeStart = tmp;
      }else{
        G.addEdge(edgeStart, tmp);
        edgeStart = -1;
      }
    }
  }


  if(ev.button.button == SDL_BUTTON_RIGHT){
    //convert to unit square coordinates of the drawing window
    if(contrStart == -1){
      for(auto& v : G.getVertices()){
        auto[vx, vy] = G.getPos(v);
        if( (x-vx) * (x-vx) + (y-vy)*(y-vy) < captureDistance) contrStart = v;
      }
    }else{
      for(auto& v : G.getVertices()){
        auto[vx, vy] = G.getPos(v);
        if( (x-vx) * (x-vx) + (y-vy)*(y-vy) < captureDistance){
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

static bool showWelcomeWindow = true;

void centerText(std::string txt, int width){
  auto tw = ImGui::CalcTextSize(txt.c_str()).x;
  ImGui::SetCursorPosX((width - tw) * 0.5f);
  ImGui::Text(txt.c_str());
}

void LLARPapp::WelcomeWindow(){
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize({(float)width,(float)height});
  ImGui::Begin(name.c_str(), &showWelcomeWindow, ImGuiWindowFlags_AlwaysAutoResize);
  centerText("Welcome to this Twin-Width drawing widget; Here are the rules:", width);
  centerText("", width);
  centerText("Press left mouse button to insert vertices, drag'n'drop to move vertices.", width);
  centerText("Press right mouse button to select vertices for contraction.", width);
  centerText("Press 'e' to toggle edge mode. In edge mode, left mouse button selects vertices to insert an edge.", width);
  centerText("Press 'n' to do greedy contraction.", width);

  centerText("", width);
  centerText("", width);
  centerText("© 2022 Too Many Small Widgets Inc. ", width);
  std::string tmp = "Get Started";
  auto w = ImGui::CalcTextSize(tmp.c_str()).x;
  ImGui::SetCursorPosX(0.5f * (width-w));
  if(ImGui::Button("Get Started")) showWelcomeWindow = false;

  ImGui::End();
}

void LLARPapp::MainWindow(){

  ImGui::SetNextWindowPos(ImVec2(width-widthOfUI, 0));
  ImGui::SetNextWindowSize({(float)widthOfUI,(float)height});


  ImGui::Begin(name.c_str(), &running, ImGuiWindowFlags_AlwaysAutoResize);


  if(ImGui::Button("Quit")) running = false;
  if(ImGui::Button("Edge Mode")) toggleEdgeMode();
  ImGui::SameLine();
  std::string tmp;
  if(EdgeMode){
    tmp = "on";
  }else{
    tmp = "off";
  }
  ImGui::Text(tmp.c_str());

  tmp = std::to_string(G.vCount()) + " Verts";
  ImGui::Text(tmp.c_str());

  tmp = std::to_string(G.maxRedDegree()) + " Maximum Red Degree";
  ImGui::Text(tmp.c_str());
  ImGui::Separator();
  ImGui::Separator();
  ImGui::Text("Generate Random Graph");
  static int n = 5;
  ImGui::InputInt("# Vertices", &n);
  n = std::clamp(n, 1, 50);
  static int dens = 33;
  ImGui::InputInt("Edge Density", &dens, 10, 90);
  dens = std::clamp(dens, 10, 90);

  if(ImGui::Button("Generate fresh graph")) G = getRandomGraph(n,dens);

  ImGui::Separator();
  ImGui::Separator();
  ImGui::Text("Generate Grid Graph");
  static int gridN = 4;
  ImGui::InputInt("Dimension", &gridN);
  gridN = std::clamp(gridN, 1, 7);
  if(ImGui::Button("Generate Grid")) G = getGrid(gridN);


  ImGui::End();



  SDL_SetRenderDrawBlendMode(SDLRenderer, SDL_BLENDMODE_BLEND);

  auto [cv,cw,cs] = G.cheapestContraction();
  for (auto u : {cv,cw}){
    SDL_SetRenderDrawColor(SDLRenderer, 0 , 255, 0, 255);

    auto [x,y] = G.getPos(u);
    SDL_Rect vx{((width - widthOfUI)*x) - 2, (height*(1.f - y)) - 2, 8, 8};
    SDL_RenderFillRect(SDLRenderer, &vx);
  }
  for(auto& v: G.getVertices()){
    if(v == vxSelected || v == contrStart || v == edgeStart){
      SDL_SetRenderDrawColor(SDLRenderer, 100 , 150, 100, 255);
    } else if(v == hoveredVx){
      SDL_SetRenderDrawColor(SDLRenderer, 150 , 200, 150, 255);
    } else {
      SDL_SetRenderDrawColor(SDLRenderer, 255 , 255, 255, 255);
    }
    auto [x,y] = G.getPos(v);
    SDL_Rect vx{((width - widthOfUI)*x) - 2, (height*(1.f - y)) - 2, 4, 4};
    SDL_RenderFillRect(SDLRenderer, &vx);
  }


  for(auto& v: G.getVertices()){
    for(auto& w : G.getVertices()){
      if(G.isEdge(v,w)){
        auto [vx,vy] = G.getPos(v);
        auto [wx,wy] = G.getPos(w);
        if(G.isRed(v,w)){
          SDL_SetRenderDrawColor(SDLRenderer, 255 , 0, 0, 255);
        }else{
          SDL_SetRenderDrawColor(SDLRenderer, 0 , 0, 255, 255);
        }
        SDL_SetRenderDrawBlendMode(SDLRenderer, SDL_BLENDMODE_BLEND);
        SDL_RenderDrawLine(SDLRenderer, (width - widthOfUI)*vx, height*(1.f - vy), (width - widthOfUI)*wx, height*(1.f - wy));
      }
    }
  }


}

LLARPapp::LLARPapp(std::string name) : name{name}{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){
    std::cerr << "SDL initialisation has failed with: " << SDL_GetError() << std::endl;
    return;
  }

    // Create window with graphics context

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_MAXIMIZED);
    SDL_DisplayMode* mode = new SDL_DisplayMode;
    SDL_GetCurrentDisplayMode(0,mode);

    window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mode->w, mode->h, window_flags);
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
      if(!showWelcomeWindow){
        if(ev.type == SDL_MOUSEBUTTONDOWN) mousePress(ev);
        if(ev.type == SDL_MOUSEBUTTONUP   && ev.button.button == SDL_BUTTON_LEFT) mouseRelease(ev);
        if(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_e) toggleEdgeMode();
        if(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_n) {auto[v,w,c] = G.cheapestContraction(); G.contract(v,w);};

      }
    }
    hovered();
    mouseMovement();



    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    SDL_SetRenderDrawColor(SDLRenderer, (Uint8)(clear_color.x * 0), (Uint8)(clear_color.y * 0), (Uint8)(clear_color.z * 0), (Uint8)(clear_color.w * 255));
    SDL_RenderClear(SDLRenderer);
    ImGui::NewFrame();
    if(showWelcomeWindow) {
      WelcomeWindow();
    }else{
      MainWindow();

    }




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
