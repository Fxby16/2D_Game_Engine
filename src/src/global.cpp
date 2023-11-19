#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <GLFW/glfw3.h>
#include <renderer.hpp>
#include <audio.hpp>

double FPS;
double CURRENT_FRAME;
double DELTA_TIME=0.0;
double LAST_FRAME=0.0;
bool V_SYNC=true;

unsigned int SCREEN_WIDTH=1600;
unsigned int SCREEN_HEIGHT=900;

unsigned int MAX_QUADS=10000;
unsigned int MAX_VERTICES=4*MAX_QUADS;
unsigned int MAX_INDICES=6*MAX_QUADS;

int DRAW_CALLS=0;

GLFWwindow *WINDOW;
bool FRAMEBUFFER_UPDATE=false;
bool PROJ_UPDATE=false;

namespace WindowInfo{
    bool ISFULLSCREEN=false;
}