#include <sceneserializer.hpp>
#include <application.hpp>

static SceneSerializer sceneserializer;

class ApplicationTemplate : public Application{
public:
    ApplicationTemplate(const char *window_name,unsigned int width,unsigned int height,float fullscreen_width,float fullscreen_height,std::string &scene_path,bool resizable=false,bool imgui=false): Application(window_name,width,height,fullscreen_width,fullscreen_height,resizable,imgui){
        sceneserializer.SetScene(m_Scene);
        sceneserializer.Deserialize(scene_path);
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