#pragma once

#include <renderer.hpp>
#include <audio.hpp>
#include <textrenderer.hpp>
#include <texture.hpp>
#include <scene.hpp>
#include <window.hpp>
#include <camera.hpp>

#include <iostream>

class Application{
public:
    Application(const char *window_name,unsigned int width,unsigned int height,bool imgui=true);
    ~Application();

    void Run();

    /**
     * This function is called every m_FixedTimeStep seconds. It's independent from the framerate.
     * Edit m_FixedTimeStep to change the time between two calls.
    */
    virtual void OnUpdate(const double frame_time);
    virtual void OnRender();
    virtual void OnImGuiUpdate();
    virtual void OnImGuiRender();

protected:
    const char *m_WindowName;
    float m_FixedTimeStep=1.0f/60.0f;
    float m_Accumulator=0.0f;
    bool m_ImGui;
    SceneManager m_SceneManager;
    Camera m_Camera;

    #ifdef DEBUG
        double last_time;
    #endif
};