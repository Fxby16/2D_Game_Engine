#pragma once

#include <array>
#include <vector>
#include <global.hpp>

struct Vec2{
    float x,y;

    float& w(){ return x; }
    float& h(){ return y; }
};

struct Vertex{
    Vec2 Position;
    Vec2 TexCoords;
    float texID;
};

class VertexBuffer{
private:
    unsigned int ID;
public:
    /**
     * Allocates memory for a vertex buffer
     * \param num_quads the number of quads to render
    */
    VertexBuffer(unsigned int num_quads);
    ~VertexBuffer();

    /**
     * Modifies data in the vertex buffer
     * \param quad_index the index of the first quad to set
     * \param data the data of the quad to set
    */
    void SetData(unsigned int quad_index,float *data,unsigned int num_quads);
    static std::array<Vertex,4> CreateQuad(float x,float y,float w,float h,float scale,float texID);
    static std::array<Vertex,4> CreateQuadSpriteSheet(float x,float y,float width,float height,float tile_width,float tile_height,float row,float col,float scale,float texID);
    void Bind() const;
    void Unbind() const;
};