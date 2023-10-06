#include <texture.hpp>

#include <stb_image.h>

Texture::Texture(const std::string &path,GLint mag_filter,GLint min_filter): ID(0),filepath(path),local_buffer(nullptr),width(0),height(0),BPP(0){
    stbi_set_flip_vertically_on_load(1);
    local_buffer=stbi_load(path.c_str(),&width,&height,&BPP,4);

    glGenTextures(1,&ID);
    glBindTexture(GL_TEXTURE_2D,ID);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,min_filter);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,mag_filter);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,local_buffer);
    
    if(local_buffer)
        stbi_image_free(local_buffer);
}

Texture::~Texture(){
    glDeleteTextures(1,&ID);
}

void Texture::Bind(unsigned int slot) const{
    glActiveTexture(GL_TEXTURE0+slot);
    glBindTexture(GL_TEXTURE_2D,ID);
}

void Texture::Unbind() const{
    glBindTexture(GL_TEXTURE_2D,0);
}

std::array<Vertex,4> SpriteSheet::CreateQuadSpriteSheet(float x,float y,float row,float col,float scale,float depth,float texID){
    Vertex v1;
    v1.Position={x,y};
    v1.TexCoords={(col*tile_width)/width,(row*tile_height)/height};
    v1.depth=depth;
    v1.texID=texID;

    Vertex v2;
    v2.Position={x,y+tile_height*scale};
    v2.TexCoords={(col*tile_width)/width,((row+1)*tile_height)/height};
    v2.depth=depth;
    v2.texID=texID;

    Vertex v3;
    v3.Position={x+tile_width*scale,y+tile_height*scale};
    v3.TexCoords={((col+1)*tile_width)/width,((row+1)*tile_height)/height};
    v3.depth=depth;
    v3.texID=texID;

    Vertex v4;
    v4.Position={x+tile_width*scale,y};
    v4.TexCoords={((col+1)*tile_width)/width,(row*tile_height)/height};
    v4.depth=depth;
    v4.texID=texID;
    
    return {v1,v2,v3,v4};
}