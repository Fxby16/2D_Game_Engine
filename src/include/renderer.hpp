#pragma once

#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <vertexarray.hpp>
#include <vertexbuffer.hpp>
#include <indexbuffer.hpp>
#include <framebuffer.hpp>
#include <shader.hpp>
#include <global.hpp>
#include <texture.hpp>

#include <glm/gtc/matrix_transform.hpp>

struct RendererData{
    VertexBuffer VBO;
    VertexArray VAO;
    VertexBufferLayout VBL;
    Shader S;
    unsigned int NumVertices;

    RendererData(const char *vertex_path,const char *fragment_path,unsigned int vertex_size);
};

enum LightType{
    ALL_LIGHT,LIGHT_AROUND_POS
};

class Renderer{
public:
    Renderer();
    ~Renderer();

    void StartScene();
    void DrawScene();

    void Clear(bool ambient_light=false) const;
    void AddLayout(VertexBufferLayout &VBL,unsigned int type,unsigned int count,bool normalized);

    void DrawTexture(float x,float y,float w,float h,float depth,float texID);
    void DrawSpriteSheet(float x,float y,float width,float height,float row,float col,float depth,SpriteSheet &s);
    void DrawAnimatedTexture(float x,float y,float width,float height,float depth,SpriteSheet &s);
    void DrawTriangle(float x1,float y1,float x2,float y2,float x3,float y3,float r,float g,float b,float a);
    void DrawQuad(Vertex v1,Vertex v2,Vertex v3,Vertex v4);
    void DrawSolidQuad(float x,float y,float w,float h,Vec4 color);
    void DrawPoint(float x,float y,float r,float g,float b,float a);
    void DrawLine(float x1,float y1,float x2,float y2,float r,float g,float b,float a);
    void DrawLight(float light_x,float light_y,Vec4 color,enum LightType light_type,float radius=0.0f,float blurAmount=0.0f);

    void Render(bool post_processing=false);
    void RenderTextures(bool post_processing);
    void RenderTriangles();
    void RenderLines();
    void RenderPoints();
    void SetPostProcessing(const char *uniform_name);
    void PostProcessing();

    void ChangeLineWidth(float new_size);
    void ChangePointSize(float new_size);
    void SetAmbientLight(Vec3 color);
    void SetClearColor(Vec3 color);

    void AddSegment(Vec2 start_point,Vec2 end_point);
    void UpdateScreenSegments();
    void ClearSegments();
    void ApplyLight();

    static void ImGui_Init();
    static void ImGui_Theme();
    static void ImGui_Start_Frame();
    static void ImGui_Content();
    static void ImGui_End_Frame();
    static void ImGui_Close();

private:
    RendererData m_Textures;
    RendererData m_Points;
    RendererData m_Lines;
    RendererData m_Triangles;

    RendererData m_Lights;

    IndexBuffer IB;

    Vertex *m_BufferT;
    LinePointVertex *m_BufferP;
    LinePointVertex *m_BufferL;
    TriangleVertex *m_BufferTR;

    Shader m_SPostProcessing;
    unsigned int m_PostProcessingIndex;
    Framebuffer *m_Framebuffer;
    Framebuffer *m_LightingFramebuffer;

    Vec3 m_AmbientLight;
    Vec3 m_ClearColor;
    
    glm::mat4 m_Proj;
    int m_Slots[32];
    int m_MaxTextureSlots;

    std::vector<std::pair<Vec2,Vec2>>segments; //used for lighting

    static bool cmp(const Vertex &v1,const Vertex &v2){
        if(v1.Depth!=v2.Depth)
            return v1.Depth>v2.Depth;
        return v1.TexID<v2.TexID; 
    }
};