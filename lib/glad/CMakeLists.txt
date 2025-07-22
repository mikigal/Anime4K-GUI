cmake_minimum_required(VERSION 3.31)
project(glad)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
add_library(${PROJECT_NAME} src/glad.c)
