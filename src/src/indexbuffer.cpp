#include <pch.hpp>
#include <indexbuffer.hpp>

IndexBuffer::IndexBuffer(){
    m_Indices=(unsigned int*)AllocateMemory(MAX_INDICES*sizeof(unsigned int));

    glGenBuffers(1,&m_ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ID);
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,MAX_INDICES*sizeof(GLuint),nullptr,GL_DYNAMIC_DRAW);
}

IndexBuffer::~IndexBuffer(){
    glDeleteBuffers(1,&m_ID);
    FreeMemory(m_Indices);
}

void IndexBuffer::Set(unsigned int num_elem){
    Bind();
    m_NumElem=num_elem/4*6;
    unsigned int offset=0;
    for(unsigned int i=0;i<m_NumElem;i+=6){
        m_Indices[i+0]=offset;
        m_Indices[i+1]=1+offset;
        m_Indices[i+2]=2+offset;

        m_Indices[i+3]=2+offset;
        m_Indices[i+4]=3+offset;
        m_Indices[i+5]=offset;

        offset+=4;
    }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0,m_NumElem*sizeof(GLuint),(const void *)m_Indices);
}

void IndexBuffer::Bind() const{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ID);
}

void IndexBuffer::Unbind() const{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}
