#pragma once

#include <camera.hpp>
#include <gizmo.hpp>

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

    void HdrWindow(bool *open);
    void ScenesWindow(bool *open);
    void GridWindow(bool *open);
    void CameraWindow(bool *open);

    void DrawGrid();

    void SerializeProject();
    void DeserializeProject();

    void HandleInputs();

    Scene *m_Scene=nullptr;
    std::vector<std::pair<std::string,uint32_t>> m_ScriptComponents; ///< Function name and entity id. will be substituted by real script components when executing
    Framebuffer *m_SceneFramebuffer=nullptr;
    SceneSerializer *m_SceneSerializer;

    std::vector<std::string> m_ScenesPaths;
    std::vector<std::vector<std::pair<std::string,uint32_t>>> m_AllScriptComponents;
    unsigned int m_SelectedScene=-1;
    std::string m_ProjectPath;
    std::string m_CWD;

    uint32_t m_SelectedEntity=std::numeric_limits<uint32_t>::max();

    std::string m_CurrentPath=std::filesystem::current_path().string();
    std::string m_EditorPath=std::filesystem::current_path().string();
    std::vector<std::filesystem::directory_entry> m_CurrentEntries;
    std::string m_SelectedFileContent;
    std::string m_SelectedFile;
    bool m_UpdateFiles;
    
    bool m_HdrOpen;
    bool m_ScenesOpen;
    bool m_GridOpen;
    bool m_CameraOpen;

    Vec2 m_CameraPos;
    bool m_ChangedCamera=false;

    Vec2 m_ScenePos;
    Vec2 m_SceneSize;

    Vec2 m_LastMousePos;
    bool m_WheelPressed=false;

    bool m_ShowGrid=false;
    bool m_SnapEnabled=false;
    float m_CellSize=1.0f;
    Vec4 m_GridColor=Vec4(0.5f,0.5f,0.5f,1.0f);

    Gizmo m_Gizmo;
};