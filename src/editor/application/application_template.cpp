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
extern TonemapType TONE_MAP_TYPE;
extern float GAMMA;
extern float EXPOSURE;

extern void LoadScripts(Scene *scene);
extern void BeforeUpdate(double frame_time,Application *app);
extern void AfterUpdate(double frame_time,Application *app);
extern void OnCreate(Application *app);
extern void OnDestroy(Application *app);

SceneSerializer sceneserializer;

class ApplicationTemplate : public Application{
public:
    ApplicationTemplate(const char *window_name,unsigned int width,unsigned int height,unsigned int fullscreen_width,unsigned int fullscreen_height,std::string &scene_path,bool resizable=false,bool imgui=false): Application(window_name,width,height,fullscreen_width,fullscreen_height,resizable,imgui){
        sceneserializer.SetScene(m_Scene);
        sceneserializer.Deserialize(scene_path);
        m_SceneName=scene_path.find_last_of('/')!=std::string::npos?scene_path.substr(scene_path.find_last_of('/')+1):scene_path;
        m_Scene->SetName(m_SceneName);

        LoadScripts(m_Scene);

        RENDERER->SetTonemapType(TONE_MAP_TYPE);
        RENDERER->SetGamma(GAMMA);
        RENDERER->SetExposure(EXPOSURE);

        RENDERER->ReloadShaders();

        OnCreate(this);
    }

    ~ApplicationTemplate(){
        OnDestroy(this);
        delete m_Scene;
    }

    void OnUpdate(double frame_time) override{
        BeforeUpdate(frame_time,this);
        m_Scene->Update(frame_time);
        AfterUpdate(frame_time,this);
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

    void LoadScene(const std::string &scene_name) override{
        if(m_Scene!=nullptr){
            delete m_Scene;
        }
        m_Scene=new Scene(scene_name);
        sceneserializer.SetScene(m_Scene);
        sceneserializer.Deserialize("resources/scenes/"+scene_name); 
        m_SceneName=scene_name;

        LoadScripts(m_Scene);          
    }
};

Application *Window::CreateApplication(){
    //parameters will be added when exporting the project
    return new ApplicationTemplate(WINDOW_NAME,WINDOW_WIDTH,WINDOW_HEIGHT,FULLSCREEN_WIDTH,FULLSCREEN_HEIGHT,SCENE_PATH,RESIZABLE);
}