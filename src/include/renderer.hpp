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

#include <glm/mat4x4.hpp>

class Camera;

struct RendererData{
    VertexBuffer VBO;
    VertexArray VAO;
    VertexBufferLayout VBL;
    Shader S;
    unsigned int NumVertices;

    RendererData(const char *vertex_path,const char *fragment_path,unsigned int vertex_size);
};

class Renderer{
public:
    Renderer();
    ~Renderer();

    void StartScene();
    void DrawScene();

    void Clear(bool ambient_light=false) const;
    void AddLayout(VertexBufferLayout &VBL,unsigned int type,unsigned int count,bool normalized);

    void DrawTexture(Vec2 pos,Vec2 size,float layer,float texID);
    /**
     * Render a portion of a texture
     * \param pos Position of the texture
     * \param size Size of the texture
     * \param texture_pos Position of the portion of the texture
     * \param texture_size Size of the portion of the texture
     * \param texture_total_size Size of the whole texture
    */
    void DrawTexture(Vec2 pos,Vec2 size,Vec2 texture_pos,Vec2 texture_size,Vec2 texture_total_size,float layer,float texID);
    void DrawTexture(Vec2 pos,Vec2 size,bool reverse_x,bool reverse_y,float layer,Texture &texture);
    void DrawSpriteSheet(Vec2 pos,Vec2 size,float row,float col,float layer,SpriteSheet &s);
    void DrawAnimatedTexture(Vec2 pos,Vec2 size,float layer,AnimatedTexture &at);
    void DrawTriangle(Vec2 pos1,Vec2 pos2,Vec2 pos3,Vec4 color,float layer);
    void DrawQuad(Vertex v1,Vertex v2,Vertex v3,Vertex v4);
    void DrawSolidQuad(Vec2 pos,Vec2 size,Vec4 color,float layer);
    void DrawPoint(Vec2 pos,Vec4 color,float layer);
    void DrawLine(Vec2 pos1,Vec2 pos2,Vec4 color,float layer);
    void DrawLight(Vec2 pos,Vec4 color,LightType light_type,float radius=0.0f,float blurAmount=0.0f);

    void Render(bool post_processing=false);
    void RenderTextures(bool post_processing,float max_layer);
    void RenderTriangles(float max_layer);
    void RenderLines(float max_layer);
    void RenderPoints(float max_layer);
    void SetPostProcessing(const char *uniform_name);
    void PostProcessing();

    void SetLineWidth(float new_size);
    void UpdateLineWidth();
    void SetPointSize(float new_size);
    float GetPointSize();
    float GetLineWidth();
    void SetAmbientLight(Vec3 color);
    void SetClearColor(Vec3 color);

    void AddSegment(Vec2 start_point,Vec2 end_point);
    void UpdateScreenSegments();
    void ClearSegments();
    std::vector<std::pair<Vec2,Vec2>> &GetSegments();
    void ApplyLight();
    void KeepCircle(Vec2 pos,float radius,float blurAmount);
    std::pair<Vec2,float> GetIntersection(const std::pair<Vec2,Vec2>&ray,const std::pair<Vec2,Vec2>&seg);

    inline RendererData* GetLightsData(){ return &m_Lights; }

    static void ImGui_Init();
    static void ImGui_Theme();
    static void ImGui_Start_Frame();
    static void ImGui_Content();
    static void ImGui_End_Frame();
    static void ImGui_Close();

private:
    friend class Camera;

    RendererData m_Textures;
    RendererData m_Points;
    RendererData m_Lines;
    RendererData m_Triangles;
    RendererData m_Lights;

    unsigned int m_TextureIndex,m_PointIndex,m_LineIndex,m_TriangleIndex;

    IndexBuffer IB;

    Vertex *m_BufferT;
    LinePointVertex *m_BufferP;
    LinePointVertex *m_BufferL;
    TriangleVertex *m_BufferTR;
    
    Framebuffer *m_Framebuffer;
    Framebuffer *m_LightingFramebuffer;
    Framebuffer *m_TempFramebuffer;

    Shader m_SPostProcessing;
    unsigned int m_PostProcessingIndex;

    Vec3 m_AmbientLight;
    Vec3 m_ClearColor;
    
    int m_Slots[32];
    int m_MaxTextureSlots;

    float m_PointSize;
    float m_LineWidth;
    float m_Zoom;

    std::vector<std::pair<Vec2,Vec2>>segments; //used for lighting

    static bool cmp1(const Vertex &v1,const Vertex &v2){
        if(v1.layer==v2.layer)
            return v1.texID<v2.texID;
        return v1.layer<v2.layer; 
    }

    static bool cmp2(const LinePointVertex &v1,const LinePointVertex &v2){
        return v1.layer<v2.layer; 
    }

    static bool cmp3(const TriangleVertex &v1,const TriangleVertex &v2){
        return v1.layer<v2.layer; 
    }

    inline float GetTexturesMinLayer(){ return m_Textures.NumVertices>0?m_BufferT[m_TextureIndex].layer:std::numeric_limits<float>::max(); }
    inline float GetPointsMinLayer(){ return m_Points.NumVertices>0?m_BufferP[m_PointIndex].layer:std::numeric_limits<float>::max(); }
    inline float GetLinesMinLayer(){ return m_Lines.NumVertices>0?m_BufferL[m_LineIndex].layer:std::numeric_limits<float>::max(); }
    inline float GetTrianglesMinLayer(){ return m_Triangles.NumVertices>0?m_BufferTR[m_TriangleIndex].layer:std::numeric_limits<float>::max(); }
};