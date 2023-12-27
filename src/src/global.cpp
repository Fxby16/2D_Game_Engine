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
bool FPS_COUNTER=false;

float BASE_SCREEN_WIDTH=1600.0f;
float BASE_SCREEN_HEIGHT=900.0f;
float SCREEN_WIDTH=1600.0f;
float SCREEN_HEIGHT=900.0f;

int DRAW_CALLS=0;

GLFWwindow *WINDOW;
bool FRAMEBUFFER_UPDATE=false;
bool PROJ_UPDATE=false;
bool TEXT_PROJ_UPDATE=false;

namespace WindowInfo{
    bool ISFULLSCREEN=false;
}