#include <pch.hpp>
#include <framebuffer.hpp>
#include <global.hpp>
#include <window.hpp>

Framebuffer::Framebuffer(){
    glGenFramebuffers(1,&m_FramebufferID);
    Bind();
    
    glGenTextures(1,&m_ColorbufferID);
    glBindTexture(GL_TEXTURE_2D,m_ColorbufferID);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16F,Window::Width,Window::Height,0,GL_RGBA,GL_FLOAT,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,m_ColorbufferID,0);

    GLenum framebuffer_status=glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(framebuffer_status!=GL_FRAMEBUFFER_COMPLETE){
        std::cerr<<"glCheckFramebufferStatus returned: "<<framebuffer_status<<'\n';
    }
}

Framebuffer::~Framebuffer(){
    glDeleteTextures(1,&m_ColorbufferID);
    glDeleteFramebuffers(1,&m_FramebufferID);
}

void Framebuffer::Bind() const{
    glBindFramebuffer(GL_FRAMEBUFFER,m_FramebufferID);
}

void Framebuffer::Unbind() const{
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}