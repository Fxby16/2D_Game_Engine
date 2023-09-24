#pragma once

#include <vector>
#include <glad/glad.h>

struct VertexBufferElement{
    unsigned int type;
    unsigned int count;
    bool normalized;

    static unsigned int GetSizeOfType(unsigned int type){
        switch(type){
        case GL_FLOAT:
            return sizeof(GLfloat);
        case GL_UNSIGNED_INT:
           return sizeof(GLuint);
        case GL_UNSIGNED_BYTE:
            return sizeof(GLbyte);
        default:
            std::__throw_runtime_error("Wrong Type\n");
            return 0;
        }
    }
};

class VertexBufferLayout{
private:
    std::vector<VertexBufferElement> Elements;
    unsigned int stride;
public:
    VertexBufferLayout() : stride(0){}

    void Push(unsigned int type,unsigned int count,bool normalized);
    inline const std::vector<VertexBufferElement> &GetElements() const{ return Elements; };
    inline unsigned int GetStride() const{ return stride; };
};