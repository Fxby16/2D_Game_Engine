#include <indexbuffer.hpp>

#include <glad/glad.h>
#include <vector>

IndexBuffer::IndexBuffer(): indices(MAX_INDICES){
    glGenBuffers(1,&ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ID);
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,MAX_INDICES*sizeof(GLuint),nullptr,GL_DYNAMIC_DRAW);
}

IndexBuffer::~IndexBuffer(){
    glDeleteBuffers(1,&ID);
}

void IndexBuffer::Set(unsigned int num_elem){
    Bind();
    NumElem=num_elem/4*6;
    unsigned int offset=0;
    for(int i=0;i<NumElem;i+=6){
        indices[i+0]=offset;
        indices[i+1]=1+offset;
        indices[i+2]=2+offset;

        indices[i+3]=2+offset;
        indices[i+4]=3+offset;
        indices[i+5]=offset;

        offset+=4;
    }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0,NumElem*sizeof(GLuint),(const void *)indices.data());
}

void IndexBuffer::Bind() const{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ID);
}

void IndexBuffer::Unbind() const{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}
