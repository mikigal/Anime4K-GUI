cmake_minimum_required(VERSION 3.31)
project(imgui)

set(CMAKE_CXX_STANDARD 20)

include_directories(./)
add_library(${PROJECT_NAME} imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp imgui_freetype.cpp backends/imgui_impl_opengl3.cpp backends/imgui_impl_glfw.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw freetype)