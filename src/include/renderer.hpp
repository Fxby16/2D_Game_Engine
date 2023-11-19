#pragma once

#include <glad/glad.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <vertexarray.hpp>
#include <vertexbuffer.hpp>
#include <indexbuffer.hpp>
#include <framebuffer.hpp>
#include <shader.hpp>
#include <global.hpp>

#include <glm/gtc/matrix_transform.hpp>

struct RendererData{
    VertexBuffer VBO;
    VertexArray VAO;
    VertexBufferLayout VBL;
    Shader S;
    unsigned int NumVertices;

    RendererData(const char *vertex_path,const char *fragment_path,unsigned int vertex_size);
};

class Renderer{
private:
    RendererData m_T;
    RendererData m_P;
    RendererData m_L;

    IndexBuffer IB;

    std::vector<Vertex>m_BufferT;
    std::vector<LinePointVertex>m_BufferP;
    std::vector<LinePointVertex>m_BufferL;

    Shader m_SPostProcessing;
    unsigned int m_PostProcessingIndex;
    Framebuffer *m_Framebuffer;
    
    glm::mat4 m_Proj;
    int m_Slots[32];
    int m_MaxTextureSlots;
public:
    Renderer();
    ~Renderer();
   
    void AddLayout(VertexBufferLayout &VBL,unsigned int type,unsigned int count,bool normalized);
    void DrawTexture(float x,float y,float w,float h,float scale,float depth,float texID);
    void DrawQuad(Vertex v1,Vertex v2,Vertex v3,Vertex v4);
    void DrawPoint(float x,float y,float r,float g,float b,float a);
    void DrawLine(float x1,float y1,float x2,float y2,float *color);

    static bool cmp(const Vertex &v1,const Vertex &v2){
        if(v1.Depth!=v2.Depth)
            return v1.Depth>v2.Depth;
        return v1.TexID<v2.TexID; 
    }

    void StartScene();
    void DrawScene();

    void Clear() const;
    void Render(bool final_scene=false);
    void SetPostProcessing(const char *uniform_name);
    void PostProcessing();

    static void ImGui_Init();
    static void ImGui_Theme();
    static void ImGui_Start_Frame();
    static void ImGui_Content();
    static void ImGui_End_Frame();
    static void ImGui_Close();
};