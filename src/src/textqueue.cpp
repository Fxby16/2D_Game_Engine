#include <pch.hpp>
#include <textqueue.hpp>

void TextQueue::Push(TextRenderer *renderer,const std::string &text,float x,float y,float scale,Vec3 color,int layer){
    m_TextQueue.push({renderer,text,x,y,scale,color,layer});
}

void TextQueue::Render(int layer){
    while(!m_TextQueue.empty() && (m_TextQueue.top().layer!=std::numeric_limits<int>::max()-1 || layer==std::numeric_limits<int>::max()-1) && m_TextQueue.top().layer<=layer){
        auto &data=m_TextQueue.top();
        data.renderer->_DrawText(data.text,data.x,data.y,data.scale,data.color);
        m_TextQueue.pop();
    }
}