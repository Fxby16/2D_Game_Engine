#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <global.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

extern bool ISFULLSCREEN;

void GLAPIENTRY MessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const void* userParam);
void GLAPIENTRY FramebufferSizeCallback(GLFWwindow *window,int width,int height);
void HandleInputs();
void GetMousePos(double *x,double *y);
int InitGlfwWindow(const char *window_name);
void ToggleFullScreen();
void PrintDebugInfo();