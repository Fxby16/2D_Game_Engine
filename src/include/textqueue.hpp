#pragma once

#include <textrenderer.hpp>

struct TextData{
    TextRenderer *renderer;
    std::string text;
    float x;
    float y;
    float scale;
    Vec3 color;
    int layer;

    TextData(TextRenderer *renderer,const std::string &text,float x,float y,float scale,Vec3 color,int layer):
        renderer(renderer),text(text),x(x),y(y),scale(scale),color(color),layer(layer){}
};

struct CompareTextData{
    bool operator()(const TextData &a,const TextData &b){
        return a.layer>b.layer;
    }
};

class TextQueue{
public:
    TextQueue()=default;

    void Push(TextRenderer *renderer,const std::string &text,float x,float y,float scale,Vec3 color,int layer);
    
    inline int GetQueueMinLayer(){
        return m_TextQueue.empty()?std::numeric_limits<int>::max():(m_TextQueue.top().layer!=std::numeric_limits<int>::max()-1?m_TextQueue.top().layer:std::numeric_limits<int>::max());
    }
    /**
     * Render all the text that has a layer <= layer.
    */
    void Render(int layer);

private:
    std::priority_queue<TextData,std::vector<TextData>,CompareTextData> m_TextQueue;
};