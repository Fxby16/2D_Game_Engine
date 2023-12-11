#pragma once

#include <string>
#include <array>

#include <vertexbuffer.hpp>
#include <glad/glad.h>

class Renderer;

class Texture{
public:
    Texture(){}
    Texture(const std::string &path,GLint mag_filter,GLint min_filter);
    ~Texture();

    void Bind(unsigned int slot=0) const;
    void Unbind() const;

    inline int GetWidth() const{ return m_Width; }
    inline int GetHeight() const{ return m_Height; }
    inline unsigned int GetTexID() const{ return m_ID; }

protected:
    unsigned int m_ID;
    std::string m_FilePath;
    unsigned char *m_LocalBuffer;
    int m_Width,m_Height,m_BPP;
};

class SpriteSheet : public Texture{
public:
    SpriteSheet(const std::string &path,unsigned int tile_width,unsigned int tile_height,GLint mag_filter,GLint min_filter)
    :Texture(path,mag_filter,min_filter),m_TileWidth(tile_width),m_TileHeight(tile_height),m_PlayAnimation(false),
    m_LoopAnimation(false),m_AnimationDelay(0),m_LastAnimationTime(0),m_AnimationIndex(0){}

    inline int GetTileWidth() const{ return m_TileWidth; }
    inline int GetTileHeight() const{ return m_TileHeight; }

    std::array<Vertex,4> CreateQuadSpriteSheet(float x,float y,float width,float height,float row,float col,float depth,float texID);
    
    void PlayAnimation(bool loop=false,float delay=0.0f);
    
    friend Renderer;
private:
    int m_TileWidth;
    int m_TileHeight;

    bool m_PlayAnimation;
    bool m_LoopAnimation;
    float m_AnimationDelay;
    float m_LastAnimationTime;
    int m_AnimationIndex;
};