#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <global.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void GLAPIENTRY MessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const void* userParam);
void GLAPIENTRY FramebufferSizeCallback(GLFWwindow *window,int width,int height);
void GLAPIENTRY KeyCallback(GLFWwindow *window,int key,int scancode,int action,int mode);
int InitGlfwWindow();