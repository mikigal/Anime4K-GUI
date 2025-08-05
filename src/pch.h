#ifndef PCH_H
#define PCH_H

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#define GLSL_VERSION "#version 330"

#define DEBUG

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <imgui.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <json.hpp>
#include <json_fwd.hpp>
#include <process.hpp>

#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <functional>

#endif //PCH_H
