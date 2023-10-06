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
    int MaxTextureSlots;
public:
    /**
     * init the renderer
     * call Render() to render textures. when you finished call draw
    */
    Renderer();
    /**
     * add attribute for the current vertex buffer
     * \param type a GL_TYPE
     * \param count the number of elements for this attribute
     * \param normalized if the data has to be normalized
    */
    void AddLayout(unsigned int type,unsigned int count,bool normalized);
    void Render(float x,float y,float w,float h,float scale,float depth,float texID);
    void Render(Vertex v1,Vertex v2,Vertex v3,Vertex v4);

    static bool cmp(const Vertex &v1,const Vertex &v2){
        if(v1.depth!=v2.depth)
            return v1.depth>v2.depth;
        return v1.texID<v2.texID; 
    }

    void Clear() const;
    void Draw();
    static void ImGui_Init(GLFWwindow *window);
    static void ImGui_Theme();
    static void ImGui_Start_Frame();
    static void ImGui_Performance();
    static void ImGui_End_Frame();
    static void ImGui_Close();
};