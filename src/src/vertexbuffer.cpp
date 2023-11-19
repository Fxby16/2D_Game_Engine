#include <vertexbuffer.hpp>

#include <glad/glad.h>

VertexBuffer::VertexBuffer(unsigned int num_vertices,unsigned int vertex_size,GLenum usage){
    glGenBuffers(1,&m_ID);
    glBindBuffer(GL_ARRAY_BUFFER,m_ID);
    glBufferData(GL_ARRAY_BUFFER,num_vertices*vertex_size,nullptr,usage); //allocate 4 vertices for every quad
}

VertexBuffer::~VertexBuffer(){
    glDeleteBuffers(1,&m_ID);
}

void VertexBuffer::Bind() const{
    glBindBuffer(GL_ARRAY_BUFFER,m_ID);
}

void VertexBuffer::Unbind() const{
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

std::array<Vertex,4> VertexBuffer::CreateQuad(float x,float y,float w,float h,float scale,float Depth,float TexID){
    Vertex v1;
    v1.Position={x,y};
    v1.TexCoords={0.0f,0.0f};
    v1.Depth=Depth;
    v1.TexID=TexID;

    Vertex v2;
    v2.Position={x,y+h*scale};
    v2.TexCoords={0.0f,1.0f};
    v2.Depth=Depth;
    v2.TexID=TexID;

    Vertex v3;
    v3.Position={x+w*scale,y+h*scale};
    v3.TexCoords={1.0f,1.0f};
    v3.Depth=Depth;
    v3.TexID=TexID;

    Vertex v4;
    v4.Position={x+w*scale,y};
    v4.TexCoords={1.0f,0.0f};
    v4.Depth=Depth;
    v4.TexID=TexID;
    
    return {v1,v2,v3,v4};
}

void VertexBuffer::SetData(unsigned int vertex_index,float *data,unsigned int num_vertices,unsigned int VertexSize){
    glBufferSubData(GL_ARRAY_BUFFER,vertex_index*sizeof(Vertex),VertexSize*num_vertices,(const void *)data);
}
