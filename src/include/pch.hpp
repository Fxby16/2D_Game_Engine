#pragma once

#include <Instrumentor.h>

//glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

//glad
#include <glad/glad.h>

//glfw
#include <GLFW/glfw3.h>

//box2d
#include <box2d/box2d.h>

//imgui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <sys/types.h>
#include <sys/sysinfo.h>

#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <source_location>
#include <algorithm>
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <map>
#include <array>
#include <limits>
#include <fstream>
#include <thread>
#include <atomic>
#include <filesystem>
#include <queue>

#include <memory.hpp>

//soloud
#include <soloud.h>
#include <soloud_wav.h>
#include <soloud_wavstream.h>

#ifdef EDITOR
    //nfd
    #include <nfd.h>
#endif

#if defined(EDITOR) || defined(APPLICATION)
    //yaml-cpp
    #include <yaml-cpp/yaml.h>
#endif