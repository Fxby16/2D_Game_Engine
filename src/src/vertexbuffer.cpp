#include <vertexbuffer.hpp>

#include <glad/glad.h>

VertexBuffer::VertexBuffer(unsigned int num_quads){
    glGenBuffers(1,&ID);
    glBindBuffer(GL_ARRAY_BUFFER,ID);
    glBufferData(GL_ARRAY_BUFFER,num_quads*4*sizeof(Vertex),nullptr,GL_DYNAMIC_DRAW); //allocate 4 vertices for every quad
}

VertexBuffer::~VertexBuffer(){
    glDeleteBuffers(1,&ID);
}

void VertexBuffer::Bind() const{
    glBindBuffer(GL_ARRAY_BUFFER,ID);
}

void VertexBuffer::Unbind() const{
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

std::array<Vertex,4> VertexBuffer::CreateQuad(float x,float y,float w,float h,float scale,float depth,float texID){
    Vertex v1;
    v1.Position={x,y};
    v1.TexCoords={0.0f,0.0f};
    v1.depth=depth;
    v1.texID=texID;

    Vertex v2;
    v2.Position={x,y+h*scale};
    v2.TexCoords={0.0f,1.0f};
    v2.depth=depth;
    v2.texID=texID;

    Vertex v3;
    v3.Position={x+w*scale,y+h*scale};
    v3.TexCoords={1.0f,1.0f};
    v3.depth=depth;
    v3.texID=texID;

    Vertex v4;
    v4.Position={x+w*scale,y};
    v4.TexCoords={1.0f,0.0f};
    v4.depth=depth;
    v4.texID=texID;
    
    return {v1,v2,v3,v4};
}

void VertexBuffer::SetData(unsigned int quad_index,float *data,unsigned int num_quads){
    glBufferSubData(GL_ARRAY_BUFFER,quad_index*sizeof(Vertex)*4,sizeof(Vertex)*4*num_quads,(const void *)data);
}
