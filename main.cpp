#include <iostream>
#include "App.hpp"
#include "graph.hpp"
#include <random>
#include <chrono>




int main(int argc, char **argv) {
  std::string name = "Twin-Width Drawing Widget";
  LLARPapp app(name);
  app.loop();
  return 0;
}
