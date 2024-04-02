#pragma once

#include <vertexarray.hpp>
#include <vertexbuffer.hpp>
#include <indexbuffer.hpp>
#include <framebuffer.hpp>
#include <shader.hpp>
#include <global.hpp>
#include <texture.hpp>
#ifdef EDITOR
    #include <editor.hpp>
#endif

class Camera;

struct RendererData{
    VertexBuffer VBO;
    VertexArray VAO;
    VertexBufferLayout VBL;
    Shader S;
    unsigned int NumVertices;

    RendererData(const char *vertex_path,const char *fragment_path,unsigned int vertex_size);
};

enum class TonemapType{
    None=0,
    Reinhard=1,
    Filmic=2,
    Uncharted2=3,
    ACES=4,
    Exponential=5,
    Logarithmic=6,
    Mantiuk=7
};

std::string TonemapTypeToString(TonemapType type);
TonemapType StringToTonemapType(const std::string &type);

class Renderer{
public:
    Renderer();
    ~Renderer();

    void StartScene();
    void DrawScene();
    #ifdef EDITOR
        friend class Editor;
        friend class SceneSerializer;

        void StartEditorScene(Editor *editor); 
        void DrawEditorScene(Framebuffer *framebuffer);
    #endif

    #ifdef APPLICATION
        friend class SceneSerializer;
    #endif

    /**
     * Clear the screen
     * \param ambient_light if true, the ambient light will be applied, else the clear color will be applied
    */
    void Clear(bool ambient_light=false) const;
    /**
     * Clear the screen
     * \param color the color to clear the screen
    */
    void Clear(Vec4 color) const;

    /**
     * Draw a texture
     * \param pos Position of the texture
     * \param size Size of the texture
     * \param layer Layer of the texture (higher layers are drawn on top of lower layers)
     * \param texID ID of the texture
    */
    void DrawTexture(Vec2 pos,Vec2 size,int layer,float texID);
    /**
     * Render a portion of a texture
     * \param pos Position of the texture
     * \param size Size of the texture
     * \param texture_pos Position of the portion of the texture
     * \param texture_size Size of the portion of the texture
     * \param texture_total_size Size of the whole texture
     * \param layer Layer of the texture (higher layers are drawn on top of lower layers)
    */
    void DrawTexture(Vec2 pos,Vec2 size,Vec2 texture_pos,Vec2 texture_size,Vec2 texture_total_size,int layer,float texID);
    /**
     * Draw a texture
     * \param pos Position of the texture
     * \param size Size of the texture
     * \param reverse_x if true, the texture will be reversed on the x axis
     * \param reverse_y if true, the texture will be reversed on the y axis
     * \param layer Layer of the texture (higher layers are drawn on top of lower layers)
     * \param texture the texture to draw
    */
    void DrawTexture(Vec2 pos,Vec2 size,bool reverse_x,bool reverse_y,int layer,Texture &texture);
    /**
     * Draw a part of a sprite sheet
     * \param pos Position of the texture
     * \param size Size of the texture
     * \param row row in the sprite sheet
     * \param col column in the sprite sheet
     * \param layer Layer of the texture (higher layers are drawn on top of lower layers)
     * \param s the sprite sheet to use
    */
    void DrawSpriteSheet(Vec2 pos,Vec2 size,float row,float col,int layer,SpriteSheet &s);
    /**
     * Draw an animated texture
     * \param pos Position of the texture
     * \param size Size of the texture
     * \param layer Layer of the texture (higher layers are drawn on top of lower layers)
     * \param s the sprite sheet to use
     * \param play_animation if true, the animation will be played
     * \param loop_animation if true, the animation will be looped
     * \param animation_delay the delay between two frames
     * \param last_animation_time the last time the animation was updated
     * \param animation_index the current animation index
    */
    void DrawAnimatedTexture(Vec2 pos,Vec2 size,int layer,SpriteSheet &s,bool &play_animation,bool loop_animation,float animation_delay,float &last_animation_time,int &animation_index);
    /**
     * Draw a triangle
     * \param pos1 Position of the first vertex
     * \param pos2 Position of the second vertex
     * \param pos3 Position of the third vertex
     * \param color Color of the triangle
     * \param layer Layer of the triangle (higher layers are drawn on top of lower layers)
    */
    void DrawTriangle(Vec2 pos1,Vec2 pos2,Vec2 pos3,Vec4 color,int layer);
    /**
     * Draw a solid quad
     * \param pos Position of the quad
     * \param size Size of the quad
     * \param color Color of the quad
     * \param layer Layer of the quad (higher layers are drawn on top of lower layers)
    */
    void DrawSolidQuad(Vec2 pos,Vec2 size,Vec4 color,int layer);
    /**
     * Draw a round point. Use SetPointSize to change the size of the point
     * \param pos Position of the point
     * \param color Color of the point
     * \param layer Layer of the point (higher layers are drawn on top of lower layers)
    */
    void DrawPoint(Vec2 pos,Vec4 color,int layer);
    /**
     * Draw an empty point. Use SetPointSize to change the size of the point
     * \param pos Position of the point
     * \param color Color of the point
     * \param border Border of the point (in range [0,1])
     * \param layer Layer of the point (higher layers are drawn on top of lower layers)
    */
    void DrawCircle(Vec2 pos,Vec4 color,float border,int layer);
    /**
     * Draw a line. Use SetLineWidth to change the size of the line
     * \param pos1 Position of the first vertex
     * \param pos2 Position of the second vertex
     * \param color Color of the line
     * \param layer Layer of the line (higher layers are drawn on top of lower layers)
    */
    void DrawLine(Vec2 pos1,Vec2 pos2,Vec4 color,int layer);
    /**
     * Draw a light
     * \param pos Position of the light
     * \param color Color of the light
     * \param light_type Type of the light
     * \param radius Radius of the light
     * \param blurAmount Amount of blur starting from the edge of the light
    */
    void DrawLight(Vec2 pos,Vec4 color,LightType light_type,float radius=0.0f,float blurAmount=0.0f);

    /**
     * Render all the elements stored in buffers
    */
    void Render(bool post_processing=false);
    /**
     * Render all the textures stored in the buffer
    */
    void RenderTextures(bool post_processing,float max_layer);
    /**
     * Render all the triangles stored in the buffer
    */
    void RenderTriangles(float max_layer);
    /**
     * Render all the lines stored in the buffer
    */
    void RenderLines(float max_layer);
    /**
     * Render all the points stored in the buffer
    */
    void RenderPoints(float max_layer);
    /**
     * Set the post processing function name in the shader
    */
    void SetPostProcessing(const char *uniform_name);
    /**
     * Apply the post processing function
    */
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
    /**
     * Apply the lighting framebuffer to the main framebuffer
    */
    void ApplyLight();
    /**
     * Merge the lighting framebuffer with the framebuffer
     * \param framebuffer the framebuffer to merge with
    */
    void ApplyLight(Framebuffer *framebuffer);
    /**
     * Keep a circle of light. Used for point lights with walls collisions
     * \param pos Position of the light
     * \param radius Radius of the light
     * \param blurAmount Amount of blur starting from the edge of the light
    */
    void KeepCircle(Vec2 pos,float radius,float blurAmount);
    /**
     * Get the intersection between a ray and a segment
     * \param ray the ray
     * \param seg the segment
     * \return the intersection point and the time of intersection if an intersection occurs, else return {(-1,-1),-1}
    */
    std::pair<Vec2,float> GetIntersection(const std::pair<Vec2,Vec2>&ray,const std::pair<Vec2,Vec2>&seg);

    inline RendererData* GetLightsData(){ return &m_Lights; }
    inline unsigned int GetFramebufferID(){ return m_Framebuffer->GetColorbufferID(); }

    void SetTonemapType(TonemapType type);
    void SetGamma(float gamma);
    void SetExposure(float exposure);
    
    TonemapType GetTonemapType();
    float GetGamma();
    float GetExposure();

    void CreateShaders();
    void ReloadShaders();
    inline void SetProjMat(glm::mat4 proj){
        m_Proj=proj;
    }
    inline void SetViewMat(glm::mat4 view){
        m_View=view;
    }
    inline void SetViewProjMat(glm::mat4 view_proj){
        m_ViewProj=view_proj;
    }

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
    PointVertex *m_BufferP;
    LineVertex *m_BufferL;
    TriangleVertex *m_BufferTR;
    
    Framebuffer *m_Framebuffer;
    Framebuffer *m_LightingFramebuffer;
    Framebuffer *m_TempFramebuffer;

    Shader m_SHdr;
    Shader m_SPostProcessing;
    unsigned int m_PostProcessingIndex;

    Vec3 m_AmbientLight;
    Vec3 m_ClearColor;
    
    int m_Slots[32];
    int m_MaxTextureSlots;

    float m_PointSize;
    float m_LineWidth;
    float m_Zoom;

    float m_Gamma;
    float m_Exposure;
    TonemapType m_TonemapType=TonemapType::Reinhard;

    //matrices set in the camera (copied here so reloading the shaders doesn't involve the camera)
    glm::mat4 m_Proj;
    glm::mat4 m_View;
    glm::mat4 m_ViewProj;

    std::vector<std::pair<Vec2,Vec2>>segments; //used for lighting

    static bool cmp1(const Vertex &v1,const Vertex &v2){
        if(v1.layer==v2.layer)
            return v1.texID<v2.texID;
        return v1.layer<v2.layer; 
    }

    template<typename T>
    static bool cmpgeneral(const T &v1,const T &v2){
        return v1.layer<v2.layer; 
    }

    inline int GetTexturesMinLayer(){ return m_Textures.NumVertices>0?m_BufferT[m_TextureIndex].layer:std::numeric_limits<float>::max(); }
    inline int GetPointsMinLayer(){ return m_Points.NumVertices>0?m_BufferP[m_PointIndex].layer:std::numeric_limits<float>::max(); }
    inline int GetLinesMinLayer(){ return m_Lines.NumVertices>0?m_BufferL[m_LineIndex].layer:std::numeric_limits<float>::max(); }
    inline int GetTrianglesMinLayer(){ return m_Triangles.NumVertices>0?m_BufferTR[m_TriangleIndex].layer:std::numeric_limits<float>::max(); }

    inline void AddLayout(VertexBufferLayout &VBL,unsigned int type,unsigned int count,bool normalized){
        VBL.Push(type,count,normalized);
    }
};