#pragma once

#include <renderer.hpp>
#include <audio.hpp>
#include <entity.hpp>
#include <textrenderer.hpp>
#include <texture.hpp>
#include <entity.hpp>
#include <glfw.hpp>

#include <iostream>


class Game{
protected:
    Renderer *m_Renderer;
    AudioPlayer *m_AudioPlayer;
    const char *m_WindowName;

public:
    Game(const char *window_name);
    ~Game();

    void Run();

    void OnUpdate(double frame_time);
    void OnRender();
    void OnImGuiUpdate();
    void OnImGuiRender();
};