#pragma once

#include <vector>
#include <global.hpp>

class IndexBuffer{
private:
    unsigned int m_ID;
    unsigned int m_NumElem;
    std::vector<unsigned int>m_Indices;
public:
    IndexBuffer();
    ~IndexBuffer();

    void Set(unsigned int num_elem);

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetNumElem() const{ return m_NumElem; }
};