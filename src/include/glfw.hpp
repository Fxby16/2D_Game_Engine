#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <global.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void GLAPIENTRY MessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const void* userParam);
void framebuffer_size_callback(GLFWwindow *window,int width,int height);
void key_callback(GLFWwindow *window,int key,int scancode,int action,int mode);
int InitGlfwWindow();