#pragma once

class Framebuffer{
public:
    Framebuffer();
    ~Framebuffer();

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetFramebufferID(){ return m_FramebufferID; }
    inline unsigned int GetColorbufferID(){ return m_ColorbufferID; }

private:
    unsigned int m_FramebufferID;
    unsigned int m_ColorbufferID;
};