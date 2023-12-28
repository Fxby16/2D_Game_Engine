#pragma once

#include <renderer.hpp>
#include <audio.hpp>
#include <textrenderer.hpp>
#include <texture.hpp>
#include <scene.hpp>
#include <window.hpp>

#include <iostream>

class Application{
public:
    Application(const char *window_name,unsigned int width,unsigned int height,bool imgui=true);
    ~Application();

    void Run();

    virtual void OnUpdate(const double frame_time);
    virtual void OnRender();
    virtual void OnImGuiUpdate();
    virtual void OnImGuiRender();

protected:
    const char *m_WindowName;
    bool imgui;
    SceneManager m_SceneManager;

    #ifdef DEBUG
        double last_time;
    #endif
};