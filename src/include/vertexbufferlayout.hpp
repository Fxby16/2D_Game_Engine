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
public:
    VertexBufferLayout() : m_Stride(0){}

    void Push(unsigned int type,unsigned int count,bool normalized);
    inline const std::vector<VertexBufferElement> &GetElements() const{ return m_Elements; };
    inline unsigned int GetStride() const{ return m_Stride; };
    inline void Clear(){
        m_Elements.clear();
        m_Stride=0;
    }
    
private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
};