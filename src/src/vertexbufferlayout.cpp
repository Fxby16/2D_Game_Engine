#include <vertexbufferlayout.hpp>

void VertexBufferLayout::Push(unsigned int type,unsigned int count,bool normalized){
    Elements.push_back({type,count,normalized});
    stride+=count*VertexBufferElement::GetSizeOfType(type);
}