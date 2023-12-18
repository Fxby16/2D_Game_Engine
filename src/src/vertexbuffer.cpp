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

std::array<Vertex,4> VertexBuffer::CreateQuad(float x,float y,float w,float h,float depth,float texID){
    Vertex v1;
    v1.position={x,y};
    v1.texcoords={0.0f,0.0f};
    v1.depth=depth;
    v1.texID=texID;

    Vertex v2;
    v2.position={x,y+h};
    v2.texcoords={0.0f,1.0f};
    v2.depth=depth;
    v2.texID=texID;

    Vertex v3;
    v3.position={x+w,y+h};
    v3.texcoords={1.0f,1.0f};
    v3.depth=depth;
    v3.texID=texID;

    Vertex v4;
    v4.position={x+w,y};
    v4.texcoords={1.0f,0.0f};
    v4.depth=depth;
    v4.texID=texID;
    
    return {v1,v2,v3,v4};
}

std::array<Vertex,4> VertexBuffer::CreateQuad(float x,float y,float w,float h,float tx,float ty,float tw,float th,float ttw,float tth,float depth,float texID){
    Vertex v1;
    v1.position={x,y};
    v1.texcoords={tx/ttw,ty/tth};
    v1.depth=depth;
    v1.texID=texID;

    Vertex v2;
    v2.position={x,y+h};
    v2.texcoords={tx/ttw,(ty+th)/tth};
    v2.depth=depth;
    v2.texID=texID;

    Vertex v3;
    v3.position={x+w,y+h};
    v3.texcoords={(tx+tw)/ttw,(ty+th)/tth};
    v3.depth=depth;
    v3.texID=texID;

    Vertex v4;
    v4.position={x+w,y};
    v4.texcoords={(tx+tw)/ttw,ty/tth};
    v4.depth=depth;
    v4.texID=texID;
    
    return {v1,v2,v3,v4};
}


void VertexBuffer::SetData(unsigned int vertex_index,float *data,unsigned int num_vertices,unsigned int VertexSize){
    glBufferSubData(GL_ARRAY_BUFFER,vertex_index*sizeof(Vertex),VertexSize*num_vertices,(const void *)data);
}
