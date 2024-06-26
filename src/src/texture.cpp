#include <pch.hpp>
#include <texture.hpp>

#include <stb_image.h>

Texture::Texture(const std::string &path,int mag_filter,int min_filter,uint32_t tex_id): m_ID(0),m_TexID(tex_id),m_MagFilter(mag_filter),m_MinFilter(min_filter),m_LocalBuffer(nullptr),m_Width(0),m_Height(0),m_BPP(0){
    m_FilePath=path.find_last_of('/')!=std::string::npos?path.substr(path.find_last_of('/')+1):path;
    m_LoadedFilePath=path;

    m_FilePath.resize(100);
    m_LoadedFilePath.resize(100);
    
    m_LoadedMagFilter=mag_filter;
    m_LoadedMinFilter=min_filter;

    stbi_set_flip_vertically_on_load(1);
    m_LocalBuffer=stbi_load(path.c_str(),&m_Width,&m_Height,&m_BPP,4);
    if(!m_LocalBuffer){
        fprintf(stderr,"Failed to load texture %s: %s\n",path.c_str(),stbi_failure_reason());
        return;
    }

    glGenTextures(1,&m_ID);
    glBindTexture(GL_TEXTURE_2D,m_ID);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,min_filter);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,mag_filter);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,m_Width,m_Height,0,GL_RGBA,GL_UNSIGNED_BYTE,m_LocalBuffer);

    stbi_image_free(m_LocalBuffer);
    m_LocalBuffer=nullptr;
}

Texture::Texture(Texture &other){
    m_ID=other.m_ID;
    other.m_ID=std::numeric_limits<unsigned int>::max();
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

Texture::~Texture(){
    if(m_ID!=std::numeric_limits<unsigned int>::max()){
        glDeleteTextures(1,&m_ID);
        if(m_LocalBuffer!=nullptr)
            free(m_LocalBuffer);
    }
}



void Texture::Bind(unsigned int slot) const{
    glActiveTexture(GL_TEXTURE0+slot);
    glBindTexture(GL_TEXTURE_2D,m_ID);
}

void Texture::Unbind() const{
    glBindTexture(GL_TEXTURE_2D,0);
}

SpriteSheet::SpriteSheet(SpriteSheet &other){
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

std::array<Vertex,4> SpriteSheet::CreateQuadSpriteSheet(float x,float y,float width,float height,float row,float col,int layer,float texID){
    Vertex v1;
    v1.position={x,y};
    v1.texcoords={(col*m_TileWidth)/m_Width,(row*m_TileHeight)/m_Height};
    v1.layer=layer;
    v1.texID=texID;

    Vertex v2;
    v2.position={x,y+height};
    v2.texcoords={(col*m_TileWidth)/m_Width,((row+1)*m_TileHeight)/m_Height};
    v2.layer=layer;
    v2.texID=texID;

    Vertex v3;
    v3.position={x+width,y+height};
    v3.texcoords={((col+1)*m_TileWidth)/m_Width,((row+1)*m_TileHeight)/m_Height};
    v3.layer=layer;
    v3.texID=texID;

    Vertex v4;
    v4.position={x+width,y};
    v4.texcoords={((col+1)*m_TileWidth)/m_Width,(row*m_TileHeight)/m_Height};
    v4.layer=layer;
    v4.texID=texID;
    
    return {v1,v2,v3,v4};
}
