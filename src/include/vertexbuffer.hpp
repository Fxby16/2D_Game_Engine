#pragma once

#include <array>
#include <vector>
#include <global.hpp>
#include <glad/glad.h>
#include <structs.hpp>

class VertexBuffer{
public:
    VertexBuffer(unsigned int num_vertices,unsigned int vertex_size,GLenum usage);
    ~VertexBuffer();

    void SetData(unsigned int vertex_index,float *data,unsigned int num_vertices,unsigned int VertexSize);
    static std::array<Vertex,4> CreateQuad(float x,float y,float w,float h,float layer,float texID);
    static std::array<Vertex,4> CreateQuad(float x,float y,float w,float h,float tx,float ty,float tw,float th,float ttw,float tth,float layer,float texID);
    void Bind() const;
    void Unbind() const;

private:
    unsigned int m_ID;
};