#pragma once

#include <camera.hpp>

class Scene;
class Framebuffer;
class SceneSerializer;

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

    void EntitiesMenu(ImVec2 pos);
    void ComponentsMenu(ImVec2 pos);

    Camera m_Camera;
    Scene *m_Scene=nullptr;
    Framebuffer *m_SceneFramebuffer=nullptr;
    SceneSerializer *m_SceneSerializer;
    std::string m_ScenePath;
    uint32_t m_SelectedEntity=std::numeric_limits<uint32_t>::max();
};