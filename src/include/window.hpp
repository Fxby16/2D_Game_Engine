#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <renderer.hpp>
#include <vector>
#include <textrenderer.hpp>

extern bool ISFULLSCREEN;
extern Renderer *RENDERER;
extern std::vector<TextRenderer*> TEXT_RENDERERS;

namespace Window{
    extern int InitGlfwWindow(const char *window_name);
    extern void DeinitGlfwWindow();

    extern void ToggleFullScreen();
    extern void ToggleVSync();

    extern bool GetKeyState(int key,int state);
    extern bool GetMouseButtonState(int button,int state);
    extern void GetMousePos(double *x,double *y);

    extern void ShowMetrics();
    extern void PrintDebugInfo();

    extern void GLAPIENTRY MessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const void* userParam);
    extern void GLAPIENTRY FramebufferSizeCallback(GLFWwindow *window,int width,int height);
    extern void GLAPIENTRY KeyCallback(GLFWwindow *window,int key,int scancode,int action,int mods);

    extern GLFWwindow *Window;

    extern float Width,Height;
    extern float BaseWidth,BaseHeight;

    extern float FPS;
    extern float CurrentFrameTime;
    extern float LastFrameTime;
    extern float DeltaTime;
    extern float Alpha; //for interpolation

    extern bool IsFullscreen;
    extern bool IsVSync;
    extern bool ShowMetrics_;

    extern bool FramebufferUpdate;
    extern bool ProjUpdate;
    extern bool TextProjUpdate;

    extern const float MAX_WIDTH;
    extern float MAX_HEIGHT;
};