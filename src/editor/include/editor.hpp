#pragma once

#include <camera.hpp>

class Scene;
class Framebuffer;
class SceneSerializer;

extern std::string WINDOW_NAME;
extern unsigned int WINDOW_WIDTH;
extern unsigned int WINDOW_HEIGHT;
extern unsigned int FULLSCREEN_WIDTH;
extern unsigned int FULLSCREEN_HEIGHT;
extern std::string SCENE_PATH;
extern bool RESIZABLE;

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
    void FileBrowserMenu(ImVec2 pos);

    void SerializeProject();
    void DeserializeProject();

    Camera m_Camera;
    Scene *m_Scene=nullptr;
    std::vector<std::pair<std::string,uint32_t>> m_ScriptComponents; ///< Function name and entity id. will be substituted by real script components when executing
    Framebuffer *m_SceneFramebuffer=nullptr;
    SceneSerializer *m_SceneSerializer;
    std::string m_ScenePath="test.scene"; //cambialo fabio
    std::string m_ProjectPath="test.proj"; //cambialo fabio
    uint32_t m_SelectedEntity=std::numeric_limits<uint32_t>::max();

    std::string m_CurrentPath=std::filesystem::current_path().string();
    std::vector<std::filesystem::directory_entry> m_CurrentEntries;
    std::string m_SelectedFileContent;
    bool m_UpdateFiles;
};