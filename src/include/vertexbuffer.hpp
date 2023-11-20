#pragma once

#include <array>
#include <vector>
#include <global.hpp>
#include <glad/glad.h>

struct Vec2{
    float x,y;

    float& w(){ return x; }
    float& h(){ return y; }
};

struct Vec3{
    float r,g,b;
};

struct Vec4{
    float r,g,b,a;
};

struct Vertex{
    Vec2 Position;
    Vec2 TexCoords;
    float Depth;
    float TexID;
};

struct LinePointVertex{
    Vec2 Pos;
    Vec4 Color;
};

class VertexBuffer{
private:
    unsigned int m_ID;
public:
    VertexBuffer(unsigned int num_vertices,unsigned int vertex_size,GLenum usage);
    ~VertexBuffer();

    void SetData(unsigned int vertex_index,float *data,unsigned int num_vertices,unsigned int VertexSize);
    static std::array<Vertex,4> CreateQuad(float x,float y,float w,float h,float scale,float depth,float texID);
    void Bind() const;
    void Unbind() const;
};