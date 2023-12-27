#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <global.hpp>
#include <buttons.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <textrenderer.hpp>
#include <renderer.hpp>

extern bool ISFULLSCREEN;
extern Renderer *RENDERER;
extern std::vector<TextRenderer*> TEXT_RENDERERS;

void GLAPIENTRY MessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const void* userParam);
void GLAPIENTRY FramebufferSizeCallback(GLFWwindow *window,int width,int height);
void GLAPIENTRY KeyCallback(GLFWwindow *window,int key,int scancode,int action,int mods);
void GetMousePos(double *x,double *y);
void ShowFpsCounter();
int InitGlfwWindow(const char *window_name);
void DeinitGlfwWindow();
void ToggleFullScreen();
void ToggleVSync();
void PrintDebugInfo();
bool getKeyState(int key,int state);
bool getMouseButtonState(int button,int state);