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

extern unsigned int SCREEN_WIDTH;
extern unsigned int SCREEN_HEIGHT;
extern unsigned int NUM_QUADS;
extern unsigned int MAX_QUADS;
extern unsigned int MAX_VERTICES;
extern unsigned int MAX_INDICES;

extern int DRAW_CALLS;

extern GLFWwindow *window;