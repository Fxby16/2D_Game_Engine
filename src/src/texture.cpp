#include <texture.hpp>

#include <stb_image.h>
#include <glad/glad.h>

Texture::Texture(const std::string &path): ID(0),filepath(path),local_buffer(nullptr),width(0),height(0),BPP(0){
    stbi_set_flip_vertically_on_load(1);
    local_buffer=stbi_load(path.c_str(),&width,&height,&BPP,4);

    glGenTextures(1,&ID);
    glBindTexture(GL_TEXTURE_2D,ID);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
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