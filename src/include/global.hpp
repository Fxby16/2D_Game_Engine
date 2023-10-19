#pragma once

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

extern double FPS;
extern double currentFrame;
extern double deltaTime;
extern double lastFrame;
extern bool v_sync;

inline constexpr unsigned int BASE_SCREEN_WIDTH=1600;
inline constexpr unsigned int BASE_SCREEN_HEIGHT=900;
extern unsigned int SCREEN_WIDTH;
extern unsigned int SCREEN_HEIGHT;
extern unsigned int MAX_QUADS;
extern unsigned int MAX_VERTICES;
extern unsigned int MAX_INDICES;

extern int DRAW_CALLS;

extern GLFWwindow *window;
extern bool framebuffer_update;
extern bool proj_update;

namespace WindowInfo{
    extern bool isFullScreen;
}