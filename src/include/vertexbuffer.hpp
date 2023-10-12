#pragma once

#include <array>
#include <vector>
#include <global.hpp>

struct Vec2{
    float x,y;

    float& w(){ return x; }
    float& h(){ return y; }
};

struct Vec4{
    float r,g,b,a;
};

struct Vertex{
    Vec2 Position;
    Vec2 TexCoords;
    float depth;
    float texID;
};

struct LinePointVertex{
    Vec2 pos;
    Vec4 color;
};

class VertexBuffer{
private:
    unsigned int ID;
public:
    /**
     * Allocates memory for a vertex buffer
     * \param num_vertices the number of quads to render
    */
    VertexBuffer(unsigned int num_vertices);
    ~VertexBuffer();

    /**
     * Modifies data in the vertex buffer
     * \param vertex_index the index of the first quad to set
     * \param data the data of the quad to set
    */
    void SetData(unsigned int vertex_index,float *data,unsigned int num_vertices,unsigned int VertexSize);
    static std::array<Vertex,4> CreateQuad(float x,float y,float w,float h,float scale,float depth,float texID);
    void Bind() const;
    void Unbind() const;
};