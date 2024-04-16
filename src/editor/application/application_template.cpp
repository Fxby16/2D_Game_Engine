#include <pch.hpp>
#include <sceneserializer.hpp>
#include <application.hpp>

extern const char *WINDOW_NAME;
extern unsigned int WINDOW_WIDTH;
extern unsigned int WINDOW_HEIGHT;
extern unsigned int FULLSCREEN_WIDTH;
extern unsigned int FULLSCREEN_HEIGHT;
extern std::string SCENE_PATH;
extern bool RESIZABLE;

extern void LoadScripts(Scene *scene);

static SceneSerializer sceneserializer;

class ApplicationTemplate : public Application{
public:
    ApplicationTemplate(const char *window_name,unsigned int width,unsigned int height,unsigned int fullscreen_width,unsigned int fullscreen_height,std::string &scene_path,bool resizable=false,bool imgui=false): Application(window_name,width,height,fullscreen_width,fullscreen_height,resizable,imgui){
        sceneserializer.SetScene(m_Scene);
        sceneserializer.Deserialize(scene_path);

        LoadScripts(m_Scene);
    }

    void OnUpdate(double frame_time) override{
        m_Scene->Update(frame_time);
    }

    void OnRender() override{
        RENDERER->StartScene();
        m_Scene->Render();
        RENDERER->Render();
        RENDERER->ApplyLight();
        #ifdef DEBUG
            m_Scene->DebugDraw();
        #endif
        m_Scene->GetCamera().DrawSceneProj();
        RENDERER->DrawScene();
        m_Scene->GetCamera().ResetSceneProj();
    }
};

Application *Window::CreateApplication(){
    //parameters will be added when exporting the project
    return new ApplicationTemplate(WINDOW_NAME,WINDOW_WIDTH,WINDOW_HEIGHT,FULLSCREEN_WIDTH,FULLSCREEN_HEIGHT,SCENE_PATH,RESIZABLE);
}