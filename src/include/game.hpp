#pragma once

#include <renderer.hpp>
#include <audio.hpp>
#include <entity.hpp>
#include <textrenderer.hpp>
#include <texture.hpp>
#include <entity.hpp>
#include <window.hpp>

#include <iostream>


class Game{
public:
    Game(const char *window_name);
    ~Game();

    void Run();

    virtual void OnUpdate(double frame_time);
    virtual void OnRender();
    virtual void OnImGuiUpdate();
    virtual void OnImGuiRender();

protected:
    Renderer *m_Renderer;
    AudioPlayer *m_AudioPlayer;
    const char *m_WindowName;

    #ifdef DEBUG
        double last_time;
    #endif
};