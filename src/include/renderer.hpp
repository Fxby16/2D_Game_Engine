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
    RendererData m_Textures;
    RendererData m_Points;
    RendererData m_Lines;
    RendererData m_Triangles;

    RendererData m_Lights;

    IndexBuffer IB;

    std::vector<Vertex>m_BufferT;
    std::vector<LinePointVertex>m_BufferP;
    std::vector<LinePointVertex>m_BufferL;
    std::vector<TriangleVertex>m_BufferTR;

    Shader m_SPostProcessing;
    unsigned int m_PostProcessingIndex;
    Framebuffer *m_Framebuffer;
    Framebuffer *m_LightingFramebuffer;
    //Framebuffer *m_TempFramebuffer;
    
    glm::mat4 m_Proj;
    int m_Slots[32];
    int m_MaxTextureSlots;
public:
    Renderer();
    ~Renderer();

    void BindLightingFB();
    void ApplyLight(const char *type,float center_x,float center_y);
   
    void AddLayout(VertexBufferLayout &VBL,unsigned int type,unsigned int count,bool normalized);
    void DrawTexture(float x,float y,float w,float h,float scale,float depth,float texID);
    void DrawTriangle(float x1,float y1,float x2,float y2,float x3,float y3,float r,float g,float b,float a);
    void DrawQuad(Vertex v1,Vertex v2,Vertex v3,Vertex v4);
    void DrawPoint(float x,float y,float r,float g,float b,float a);
    void DrawLine(float x1,float y1,float x2,float y2,float r,float g,float b,float a);

    void ChangeLineWidth(float new_size);
    void ChangePointSize(float new_size);

    static bool cmp(const Vertex &v1,const Vertex &v2){
        if(v1.Depth!=v2.Depth)
            return v1.Depth>v2.Depth;
        return v1.TexID<v2.TexID; 
    }

    void StartScene();
    void DrawScene();

    void Clear(Vec3 color) const;
    void Render(bool post_processing=false);
    void RenderTextures(bool post_processing);
    void RenderTriangles();
    void RenderLines();
    void RenderPoints();
    void SetPostProcessing(const char *uniform_name);
    void PostProcessing();

    static void ImGui_Init();
    static void ImGui_Theme();
    static void ImGui_Start_Frame();
    static void ImGui_Content();
    static void ImGui_End_Frame();
    static void ImGui_Close();
};