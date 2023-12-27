#pragma once

#include <renderer.hpp>
#include <audio.hpp>
#include <entity.hpp>
#include <textrenderer.hpp>
#include <texture.hpp>
#include <entity.hpp>
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
    std::vector<Entity> m_Entities;
    bool imgui;

    #ifdef DEBUG
        double last_time;
    #endif
};