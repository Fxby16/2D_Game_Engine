#pragma once

#include <vertexbuffer.hpp>
#include <vertexbufferlayout.hpp>

class VertexArray{
private:
    unsigned int ID;
public:
    VertexArray();
    ~VertexArray();

    void AddBuffer(const VertexBuffer &vb,const VertexBufferLayout &layout);

    void Bind() const;
    void Unbind() const;
};