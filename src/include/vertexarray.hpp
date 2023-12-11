#pragma once

#include <vertexbuffer.hpp>
#include <vertexbufferlayout.hpp>

class VertexArray{
public:
    VertexArray();
    ~VertexArray();

    void AddBuffer(const VertexBuffer &vb,const VertexBufferLayout &layout);
    void DisableAttribs(const VertexBufferLayout &layout);

    void Bind() const;
    void Unbind() const;

private:
    unsigned int m_ID;
};