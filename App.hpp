#ifndef LLLRAP_APP_HPP
#define LLLRAP_APP_HPP

#include <vector>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include <stdio.h>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif
#include "graph.hpp"

class LLARPapp {
private:
  SDL_Window* window;
  SDL_GLContext gl_context;
  SDL_Renderer* SDLRenderer = nullptr;

  int width;
  int height;

  std::string name;
  Graph G;

  ImGuiIO io;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


  bool running = false;

  void dataDrawingWindow();
  void MainWindow();
  void mousePress(SDL_Event& ev);
  void mouseRelease(SDL_Event& ev);
  void mouseMovement();
  void hovered();

public:
  void loop();
  LLARPapp (std::string name);
  virtual ~LLARPapp ();
};


#endif
