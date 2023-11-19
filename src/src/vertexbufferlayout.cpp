#include <vertexbufferlayout.hpp>

void VertexBufferLayout::Push(unsigned int type,unsigned int count,bool normalized){
    m_Elements.push_back({type,count,normalized});
    m_Stride+=count*VertexBufferElement::GetSizeOfType(type);
}