#pragma once

#include <camera.hpp>

class Scene;
class Framebuffer;
class SceneSerializer;

class Editor{
public:
    Editor(unsigned int width,unsigned int height,float fullscreen_width,float fullscreen_height,bool resizable=false);
    ~Editor();

    void Run();
    void OnImGuiUpdate();
    void OnImGuiRender();   
    void OnSceneRender();
    void HighlightEntity(uint32_t uid);
private:   
    friend class Renderer;

    void EntitiesMenu(ImVec2 pos);
    void ComponentsMenu(ImVec2 pos);
    void DataMenu(ImVec2 pos);
    void VariablesMenu(ImVec2 pos);

    Camera m_Camera;
    Scene *m_Scene=nullptr;
    std::vector<std::pair<std::string,uint32_t>> m_ScriptComponents; ///< Function name and entity id. will be substituted by real script components when executing
    Framebuffer *m_SceneFramebuffer=nullptr;
    SceneSerializer *m_SceneSerializer;
    std::string m_ScenePath;
    uint32_t m_SelectedEntity=std::numeric_limits<uint32_t>::max();
};