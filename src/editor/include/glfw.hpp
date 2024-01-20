#pragma once

#include <global.hpp>

int InitGlfwWindow(const char *window_name);
void GLAPIENTRY MessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const void* userParam);
void GLAPIENTRY FramebufferSizeCallback(GLFWwindow *window,int width,int height);