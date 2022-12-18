cmake_minimum_required(VERSION 3.10)



message(STATUS "vcpkg at $ENV{VCPKG_HOME}")
set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_HOME}) ## Set VCPKG_HOME to point to vcpkg.cmake
set(VCPKG_TARGET_TRIPLET "x64-windows")

project(TwinWidthDrawer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_package(SDL2 CONFIG REQUIRED)

set(IMGUI_PATH "${CMAKE_CURRENT_SOURCE_DIR}/imgui")


set(IMGUI_SOURCE ${IMGUI_PATH}/imgui.cpp ${IMGUI_PATH}/imgui_draw.cpp ${IMGUI_PATH}/imgui_tables.cpp ${IMGUI_PATH}/imgui_widgets.cpp ${IMGUI_PATH}/backends/imgui_impl_sdl.cpp ${IMGUI_PATH}/backends/imgui_impl_sdlrenderer.cpp)


add_executable(TWDraw main.cpp graph.cpp App.cpp ${IMGUI_SOURCE})
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${GUROBI_INCLUDE_DIRS} ${IMGUI_PATH} ${IMGUI_PATH}/backends  ${SDL2_INCLUDE_DIRS})
target_link_libraries(TWDraw ${GUROBI_CXX_DEBUG_LIBRARY} ${GUROBI_LIBRARY} ${SDL2_LIBRARIES})
