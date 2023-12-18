#include <texture.hpp>

#include <stb_image.h>

Texture::Texture(const std::string &path,GLint mag_filter,GLint min_filter): m_ID(0),m_FilePath(path),m_LocalBuffer(nullptr),m_Width(0),m_Height(0),m_BPP(0){
    stbi_set_flip_vertically_on_load(1);
    m_LocalBuffer=stbi_load(path.c_str(),&m_Width,&m_Height,&m_BPP,4);

    glGenTextures(1,&m_ID);
    glBindTexture(GL_TEXTURE_2D,m_ID);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,min_filter);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,mag_filter);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,m_Width,m_Height,0,GL_RGBA,GL_UNSIGNED_BYTE,m_LocalBuffer);
    
    if(m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);
}

Texture::~Texture(){
    glDeleteTextures(1,&m_ID);
}

void Texture::Bind(unsigned int slot) const{
    glActiveTexture(GL_TEXTURE0+slot);
    glBindTexture(GL_TEXTURE_2D,m_ID);
}

void Texture::Unbind() const{
    glBindTexture(GL_TEXTURE_2D,0);
}

std::array<Vertex,4> SpriteSheet::CreateQuadSpriteSheet(float x,float y,float width,float height,float row,float col,float depth,float texID){
    Vertex v1;
    v1.position={x,y};
    v1.texcoords={(col*m_TileWidth)/m_Width,(row*m_TileHeight)/m_Height};
    v1.depth=depth;
    v1.texID=texID;

    Vertex v2;
    v2.position={x,y+height};
    v2.texcoords={(col*m_TileWidth)/m_Width,((row+1)*m_TileHeight)/m_Height};
    v2.depth=depth;
    v2.texID=texID;

    Vertex v3;
    v3.position={x+width,y+height};
    v3.texcoords={((col+1)*m_TileWidth)/m_Width,((row+1)*m_TileHeight)/m_Height};
    v3.depth=depth;
    v3.texID=texID;

    Vertex v4;
    v4.position={x+width,y};
    v4.texcoords={((col+1)*m_TileWidth)/m_Width,(row*m_TileHeight)/m_Height};
    v4.depth=depth;
    v4.texID=texID;
    
    return {v1,v2,v3,v4};
}

void SpriteSheet::PlayAnimation(bool loop,float delay){
    m_PlayAnimation=true;
    m_LoopAnimation=loop;
    m_AnimationDelay=delay;
}
