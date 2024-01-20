#pragma once

#include <camera.hpp>
#include <framebuffer.hpp>

class Editor{
public:
    Editor(unsigned int width,unsigned int height,float fullscreen_width,float fullscreen_height);
    ~Editor();

    void Run();
    void OnImGuiUpdate();
    void OnImGuiRender();   
    void OnSceneRender();
private:   
    friend class Renderer;

    Camera m_Camera;
    Framebuffer *m_SceneFramebuffer=nullptr;
};