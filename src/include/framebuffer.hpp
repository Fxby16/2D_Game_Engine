#pragma once

class Framebuffer{
private:
    unsigned int m_FramebufferID;
    unsigned int m_ColorbufferID;
public:
    Framebuffer();
    ~Framebuffer();

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetFramebufferID(){ return m_FramebufferID; }
    inline unsigned int GetColorbufferID(){ return m_ColorbufferID; }
};