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

class Renderer{
private:
    VertexBuffer VB_T; //vertex buffer for textures
    VertexBuffer VB_P; //vertex buffer for points
    VertexBuffer VB_L; //vertex buffer for lines
    VertexArray VA_T; //vertex array for textures
    VertexArray VA_P; //vertex array for points
    VertexArray VA_L; //vertex array for lines
    IndexBuffer IB;
    VertexBufferLayout VBL_T; //vertex buffer layout for textures
    VertexBufferLayout VBL_P; //vertex buffer layout for points
    VertexBufferLayout VBL_L; //vertex buffer layout for lines
    std::vector<Vertex>buffer_T; //buffer for textures
    std::vector<LinePointVertex>buffer_P; //buffer for point
    std::vector<LinePointVertex>buffer_L; //buffer for lines
    Shader shader_T; //shader for textures
    Shader shader_P; //shader for round points
    Shader shader_L; //shader for lines
    Shader shader_post_processing;
    unsigned int postprocessing_index;
    Framebuffer framebuffer;
    
    unsigned int Num_Vertices_T;
    unsigned int Num_Vertices_P;
    unsigned int Num_Vertices_L;
    glm::mat4 proj;
    int slots[32];
    int MaxTextureSlots;
public:
    /**
     * init the renderer
     * call Render() to render textures. when you finished call Draw()
    */
    Renderer();
    /**
     * add attribute for the current vertex buffer
     * \param type a GL_TYPE
     * \param count the number of elements for this attribute
     * \param normalized if the data has to be normalized
    */
   /**
    * add vertex buffer attribute. call VertexArray::AddBuffer() to apply the attributes to the vertex array
   */
    void AddLayout(VertexBufferLayout &VBL,VertexArray &VA,unsigned int type,unsigned int count,bool normalized);
    void RenderTexture(float x,float y,float w,float h,float scale,float depth,float texID);
    void RenderQuad(Vertex v1,Vertex v2,Vertex v3,Vertex v4);
    void RenderPoint(float x,float y,float r,float g,float b,float a);
    void RenderLine(float x1,float y1,float x2,float y2,float *color);

    static bool cmp(const Vertex &v1,const Vertex &v2){
        if(v1.depth!=v2.depth)
            return v1.depth>v2.depth;
        return v1.texID<v2.texID; 
    }

    void StartScene();
    void DrawScene();

    void Clear() const;
    void Draw(bool finalScene=false);
    void SetPostProcessing(const char *uniformName);
    void PostProcessing();

    static void ImGui_Init(GLFWwindow *window);
    static void ImGui_Theme();
    static void ImGui_Start_Frame();
    static void ImGui_Performance();
    static void ImGui_End_Frame();
    static void ImGui_Close();
};