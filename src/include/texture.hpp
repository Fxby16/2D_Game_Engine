#pragma once

#include <vertexbuffer.hpp>

class Renderer;
class Entity;
class Scene;

class Texture{
public:
    Texture(): m_ID(std::numeric_limits<unsigned int>::max()),m_TexID(std::numeric_limits<uint32_t>::max()),m_FilePath(100,'\0'),m_LocalBuffer(nullptr),
        m_Width(0),m_Height(0),m_BPP(0){}
    Texture(const std::string &path,int mag_filter,int min_filter,uint32_t texID);
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
        m_TexID=other.m_TexID;
        m_MagFilter=other.m_MagFilter;
        m_MinFilter=other.m_MinFilter;
        m_FilePath=other.m_FilePath;
        m_LoadedFilePath=other.m_LoadedFilePath;
        m_LoadedMagFilter=other.m_LoadedMagFilter;
        m_LoadedMinFilter=other.m_LoadedMinFilter;
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
            m_TexID=other.m_TexID;
            m_MagFilter=other.m_MagFilter;
            m_MinFilter=other.m_MinFilter;
            m_FilePath=other.m_FilePath;
            m_LoadedFilePath=other.m_LoadedFilePath;
            m_LoadedMagFilter=other.m_LoadedMagFilter;
            m_LoadedMinFilter=other.m_LoadedMinFilter;
            m_LocalBuffer=other.m_LocalBuffer;
            m_Width=other.m_Width;
            m_Height=other.m_Height;
            m_BPP=other.m_BPP;
        }
        return *this;
    }

    inline void FreeTexture(){
        if(m_ID!=std::numeric_limits<unsigned int>::max()){
            glDeleteTextures(1,&m_ID);
            if(m_LocalBuffer!=nullptr)
                free(m_LocalBuffer);
        }
    }
protected:
    friend class Renderer;
    friend class TexturesManager;
    friend class SceneSerializer;
    friend class Scene;

    #ifdef EDITOR
        friend class Editor;
    #endif

    unsigned int m_ID;
    uint32_t m_TexID;
    std::string m_FilePath,m_LoadedFilePath;
    int m_MagFilter,m_MinFilter,m_LoadedMagFilter,m_LoadedMinFilter;
    unsigned char *m_LocalBuffer;
    int m_Width,m_Height,m_BPP;
};

class SpriteSheet : public Texture{
public:
    SpriteSheet(const std::string &path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter,uint32_t tex_id)
    :Texture(path,mag_filter,min_filter,tex_id),m_TileWidth(tile_width),m_TileHeight(tile_height){}
    SpriteSheet(): Texture(),m_TileWidth(0),m_TileHeight(0){}
    SpriteSheet(SpriteSheet &other);

    SpriteSheet &operator=(SpriteSheet &other){
        m_ID=other.m_ID;
        other.m_ID=std::numeric_limits<unsigned int>::max();
        m_TexID=other.m_TexID;
        m_MagFilter=other.m_MagFilter;
        m_MinFilter=other.m_MinFilter;
        m_FilePath=other.m_FilePath;
        m_LoadedFilePath=other.m_LoadedFilePath;
        m_LoadedMagFilter=other.m_LoadedMagFilter;
        m_LoadedMinFilter=other.m_LoadedMinFilter;
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
            m_TexID=other.m_TexID;
            m_MagFilter=other.m_MagFilter;
            m_MinFilter=other.m_MinFilter;
            m_FilePath=other.m_FilePath;
            m_LoadedFilePath=other.m_LoadedFilePath;
            m_LoadedMagFilter=other.m_LoadedMagFilter;
            m_LoadedMinFilter=other.m_LoadedMinFilter;
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

    std::array<Vertex,4> CreateQuadSpriteSheet(float x,float y,float width,float height,float row,float col,int layer,float texID);
    
private:
    friend class Renderer;
    friend class TexturesManager;
    friend class SceneSerializer;
    friend class Scene;
    #if defined(EDITOR) || defined(APPLICATION)
        friend class Editor;
    #endif

    int m_TileWidth;
    int m_TileHeight;
};