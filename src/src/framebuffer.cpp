#include <framebuffer.hpp>
#include <glad/glad.h>
#include <global.hpp>
#include <iostream>

Framebuffer::Framebuffer(){
    glGenFramebuffers(1,&framebufferID);
    Bind();
    
    glGenTextures(1,&colorbufferID);
    glBindTexture(GL_TEXTURE_2D,colorbufferID);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,SCREEN_WIDTH,SCREEN_HEIGHT,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,colorbufferID,0);

    GLenum framebuffer_status=glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(framebuffer_status!=GL_FRAMEBUFFER_COMPLETE){
        std::cout<<"glCheckFramebufferStatus returned: "<<framebuffer_status<<'\n';
    }
}

Framebuffer::~Framebuffer(){
    glDeleteTextures(1,&colorbufferID);
    glDeleteFramebuffers(1,&framebufferID);
}

void Framebuffer::Bind() const{
    glBindFramebuffer(GL_FRAMEBUFFER,framebufferID);
}

void Framebuffer::Unbind() const{
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}