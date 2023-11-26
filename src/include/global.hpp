#pragma once

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

extern double FPS;
extern double CURRENT_FRAME;
extern double DELTA_TIME;
extern double LAST_FRAME;
extern bool V_SYNC;

inline constexpr unsigned int BASE_SCREEN_WIDTH=1600;
inline constexpr unsigned int BASE_SCREEN_HEIGHT=900;
extern unsigned int SCREEN_WIDTH;
extern unsigned int SCREEN_HEIGHT;

inline constexpr unsigned int MAX_QUADS=5000;
inline constexpr unsigned int MAX_VERTICES=4*MAX_QUADS;
inline constexpr unsigned int MAX_INDICES=6*MAX_QUADS;

extern int DRAW_CALLS;

extern GLFWwindow *WINDOW;
extern bool FRAMEBUFFER_UPDATE;
extern bool PROJ_UPDATE;

namespace WindowInfo{
    extern bool ISFULLSCREEN;
}