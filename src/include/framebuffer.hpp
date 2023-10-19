#pragma once

class Framebuffer{
private:
    unsigned int framebufferID;
    unsigned int colorbufferID;
public:
    Framebuffer();
    ~Framebuffer();

    void Bind() const;
    void Unbind() const;

    inline unsigned int getFramebufferID(){ return framebufferID; }
    inline unsigned int getColorbufferID(){ return colorbufferID; }
};