#pragma once

#include <glad/glad.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <vertexarray.hpp>
#include <vertexbuffer.hpp>
#include <indexbuffer.hpp>
#include <shader.hpp>
#include <global.hpp>

#include <glm/gtc/matrix_transform.hpp>

class Renderer{
private:
    VertexBuffer VB;
    VertexArray VA;
    IndexBuffer IB;
    VertexBufferLayout VBL;
    std::vector<Vertex>buffer;
    unsigned int Num_Vertices;
    Shader base_shader;
    glm::mat4 proj;
    int slots[32];
public:
    Renderer();
    void AddLayout(unsigned int type,unsigned int count,bool normalized);
    void Render(float x,float y,float w,float h,float texID);

    void GetMaxTextureSlots();
    void Clear() const;
    void Draw();
    static void ImGui_Init(GLFWwindow *window);
    static void ImGui_Start_Frame();
    static void ImGui_Body();
    static void ImGui_End_Frame();
    static void ImGui_Close();
};