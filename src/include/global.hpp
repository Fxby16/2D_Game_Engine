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
extern bool FPS_COUNTER;

extern float BASE_SCREEN_WIDTH;
extern float BASE_SCREEN_HEIGHT;
extern float SCREEN_WIDTH;
extern float SCREEN_HEIGHT;

inline constexpr unsigned int MAX_QUADS=5000;
inline constexpr unsigned int MAX_VERTICES=4*MAX_QUADS;
inline constexpr unsigned int MAX_INDICES=6*MAX_QUADS;

extern int DRAW_CALLS;

extern GLFWwindow *WINDOW;
extern bool FRAMEBUFFER_UPDATE;
extern bool PROJ_UPDATE;
extern bool TEXT_PROJ_UPDATE;

namespace WindowInfo{
    extern bool ISFULLSCREEN;
}

enum LightType{
    ALL_LIGHT,LIGHT_AROUND_POS,LIGHT_AROUND_POS_COLL
};