#pragma once

#include <vector>
#include <global.hpp>

class IndexBuffer{
private:
    unsigned int ID;
    unsigned int NumElem;
    std::vector<unsigned int>indices;
public:
    IndexBuffer();
    ~IndexBuffer();

    void Set(unsigned int num_elem);

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetNumElem() const{ return NumElem; }
};