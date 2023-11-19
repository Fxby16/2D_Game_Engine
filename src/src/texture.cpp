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

std::array<Vertex,4> SpriteSheet::CreateQuadSpriteSheet(float x,float y,float row,float col,float scale,float Depth,float texID){
    Vertex v1;
    v1.Position={x,y};
    v1.TexCoords={(col*m_TileWidth)/m_Width,(row*m_TileHeight)/m_Height};
    v1.Depth=Depth;
    v1.TexID=texID;

    Vertex v2;
    v2.Position={x,y+m_TileHeight*scale};
    v2.TexCoords={(col*m_TileWidth)/m_Width,((row+1)*m_TileHeight)/m_Height};
    v2.Depth=Depth;
    v2.TexID=texID;

    Vertex v3;
    v3.Position={x+m_TileWidth*scale,y+m_TileHeight*scale};
    v3.TexCoords={((col+1)*m_TileWidth)/m_Width,((row+1)*m_TileHeight)/m_Height};
    v3.Depth=Depth;
    v3.TexID=texID;

    Vertex v4;
    v4.Position={x+m_TileWidth*scale,y};
    v4.TexCoords={((col+1)*m_TileWidth)/m_Width,(row*m_TileHeight)/m_Height};
    v4.Depth=Depth;
    v4.TexID=texID;
    
    return {v1,v2,v3,v4};
}