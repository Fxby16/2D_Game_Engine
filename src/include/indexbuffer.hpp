#pragma once

#include <global.hpp>

class IndexBuffer{
public:
    IndexBuffer();
    ~IndexBuffer();

    void Set(unsigned int num_elem);

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetNumElem() const{ return m_NumElem; }

private:
    unsigned int m_ID;
    unsigned int m_NumElem;
    unsigned int *m_Indices;
};