#pragma once

#include <string>
#include <array>

#include <vertexbuffer.hpp>
#include <glad/glad.h>
#include <limits>

class Renderer;

class Texture{
public:
    Texture(): m_ID(std::numeric_limits<unsigned int>::max()),m_FilePath(),m_LocalBuffer(nullptr),
        m_Width(0),m_Height(0),m_BPP(0){}
    Texture(const std::string &path,int mag_filter,int min_filter);
    Texture(Texture &other);
    ~Texture();

    void Bind(unsigned int slot=0) const;
    void Unbind() const;

    inline int GetWidth() const{ return m_Width; }
    inline int GetHeight() const{ return m_Height; }
    inline unsigned int GetTexID() const{ return m_ID; }
    
    Texture &operator=(Texture &other){
        m_ID=other.m_ID;
        other.m_ID=std::numeric_limits<unsigned int>::max();
        m_FilePath=other.m_FilePath;
        m_LocalBuffer=other.m_LocalBuffer;
        m_Width=other.m_Width;
        m_Height=other.m_Height;
        m_BPP=other.m_BPP;
        return *this;
    }

    Texture &operator=(Texture &&other){
        if(this!=&other){
            m_ID=other.m_ID;
            other.m_ID=std::numeric_limits<unsigned int>::max();
            m_FilePath=other.m_FilePath;
            m_LocalBuffer=other.m_LocalBuffer;
            m_Width=other.m_Width;
            m_Height=other.m_Height;
            m_BPP=other.m_BPP;
        }
        return *this;
    }
protected:
    unsigned int m_ID;
    std::string m_FilePath;
    unsigned char *m_LocalBuffer;
    int m_Width,m_Height,m_BPP;
};

class SpriteSheet : public Texture{
public:
    SpriteSheet(const std::string &path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter)
    :Texture(path,mag_filter,min_filter),m_TileWidth(tile_width),m_TileHeight(tile_height){}
    SpriteSheet(): Texture(),m_TileWidth(0),m_TileHeight(0){}
    SpriteSheet(SpriteSheet &other);

    SpriteSheet &operator=(SpriteSheet &other){
        m_ID=other.m_ID;
        other.m_ID=std::numeric_limits<unsigned int>::max();
        m_FilePath=other.m_FilePath;
        m_LocalBuffer=other.m_LocalBuffer;
        m_Width=other.m_Width;
        m_Height=other.m_Height;
        m_BPP=other.m_BPP;
        m_TileWidth=other.m_TileWidth;
        m_TileHeight=other.m_TileHeight;
        return *this;
    }

    SpriteSheet &operator=(SpriteSheet &&other){
        if(this!=&other){
            m_ID=other.m_ID;
            other.m_ID=std::numeric_limits<unsigned int>::max();
            m_FilePath=other.m_FilePath;
            m_LocalBuffer=other.m_LocalBuffer;
            m_Width=other.m_Width;
            m_Height=other.m_Height;
            m_BPP=other.m_BPP;
            m_TileWidth=other.m_TileWidth;
            m_TileHeight=other.m_TileHeight;
        }
        return *this;
    }

    inline int GetTileWidth() const{ return m_TileWidth; }
    inline int GetTileHeight() const{ return m_TileHeight; }

    std::array<Vertex,4> CreateQuadSpriteSheet(float x,float y,float width,float height,float row,float col,float layer,float texID);
    
protected:
    int m_TileWidth;
    int m_TileHeight;
};

class AnimatedTexture : public SpriteSheet{
public:
    AnimatedTexture(const std::string &path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter):
    SpriteSheet(path,tile_width,tile_height,mag_filter,min_filter),m_PlayAnimation(false),m_LoopAnimation(false),m_AnimationDelay(0.0f),m_LastAnimationTime(0.0f),m_AnimationIndex(0){}
    AnimatedTexture(): SpriteSheet(),m_PlayAnimation(false),m_LoopAnimation(false),m_AnimationDelay(0.0f),m_LastAnimationTime(0.0f),m_AnimationIndex(0){}
    AnimatedTexture(AnimatedTexture &other);

    AnimatedTexture &operator=(AnimatedTexture &other){
        m_ID=other.m_ID;
        other.m_ID=std::numeric_limits<unsigned int>::max();
        m_FilePath=other.m_FilePath;
        m_LocalBuffer=other.m_LocalBuffer;
        m_Width=other.m_Width;
        m_Height=other.m_Height;
        m_BPP=other.m_BPP;
        m_TileWidth=other.m_TileWidth;
        m_TileHeight=other.m_TileHeight;
        m_PlayAnimation=other.m_PlayAnimation;
        m_LoopAnimation=other.m_LoopAnimation;
        m_AnimationDelay=other.m_AnimationDelay;
        m_LastAnimationTime=other.m_LastAnimationTime;
        m_AnimationIndex=other.m_AnimationIndex;
        return *this;
    }

    AnimatedTexture &operator=(AnimatedTexture &&other){
        if(this!=&other){
            m_ID=other.m_ID;
            other.m_ID=std::numeric_limits<unsigned int>::max();
            m_FilePath=other.m_FilePath;
            m_LocalBuffer=other.m_LocalBuffer;
            m_Width=other.m_Width;
            m_Height=other.m_Height;
            m_BPP=other.m_BPP;
            m_TileWidth=other.m_TileWidth;
            m_TileHeight=other.m_TileHeight;
            m_LoopAnimation=other.m_LoopAnimation;
            m_AnimationDelay=other.m_AnimationDelay;
            m_LastAnimationTime=other.m_LastAnimationTime;
            m_AnimationIndex=other.m_AnimationIndex;
        }
        return *this;
    }

    void PlayAnimation(bool loop=false,float delay=0.0f);

    friend Renderer;
private:
    bool m_PlayAnimation;
    bool m_LoopAnimation;
    float m_AnimationDelay;
    float m_LastAnimationTime;
    int m_AnimationIndex;
};