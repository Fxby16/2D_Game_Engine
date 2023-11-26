#pragma once

#include <array>
#include <vector>
#include <global.hpp>
#include <glad/glad.h>

struct Vec2{
    union{
        float x;
        float w;
    };
    union{
        float y;
        float h;
    };
    Vec2(): x(0),y(0){}
    Vec2(float x,float y): x(x),y(y){}
};

struct Vec3{
    float r,g,b;
};

struct Vec4{
    float r,g,b,a;
    Vec4(): r(0),g(0),b(0),a(0){}
    Vec4(float r,float g,float b,float a): r(r),g(g),b(b),a(a){}
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

struct TriangleVertex{
    Vec2 Position;
    Vec4 Color;
    TriangleVertex(): Position(Vec2()),Color(Vec4()){}
    TriangleVertex(Vec2 pos,Vec4 color): Position(pos),Color(color){}
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