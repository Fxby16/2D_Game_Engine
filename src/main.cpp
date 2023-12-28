#include <application.hpp>

class Test : public Application{
public:
    Test(const char *title,int width,int height,bool imgui=false): Application(title,width,height,imgui){
        m_SceneManager.AddScene("Test");
        m_SceneManager.SetCurrentScene("Test");
        Scene *scene=m_SceneManager.GetCurrentScene();

        m_Entities.push_back(scene->AddEntity());
        scene->AddComponent<TextureComponent>(m_Entities[0],"/home/fabio/Scaricati/Red.png",GL_LINEAR,GL_LINEAR,100.0f,100.0f,0.0f);
        scene->AddComponent<ColliderComponent>(m_Entities[0],100.0f,100.0f,0.0f,0.0f,0.0f,0.0f);
        scene->AddComponent<LightComponent>(m_Entities[0],0.0f,0.0f,200.0f,0.3f,Vec3(1,1,1),LIGHT_AROUND_POS);
        scene->GetComponent<LightComponent>(m_Entities[0])->SetCentered(100.0f,100.0f);

        m_Entities.push_back(scene->AddEntity());
        scene->AddComponent<AnimatedTextureComponent>(m_Entities[1],"resources/textures/Run.png",(unsigned int)128,(unsigned int)128,GL_NEAREST,GL_NEAREST,300.0f,300.0f,0.0f);
        scene->AddComponent<ColliderComponent>(m_Entities[1],300.0f,200.0f,0.0f,0.0f,0.0f,0.0f);
        scene->SetEntityPosition(m_Entities[1],500,500);

        m_SceneManager.AddScene("Test2");
        m_SceneManager.SetCurrentScene("Test2");
        scene=m_SceneManager.GetCurrentScene();

        m_Entities.push_back(scene->AddEntity());
        scene->AddComponent<TextureComponent>(m_Entities[2],"/home/fabio/Scaricati/Red.png",GL_LINEAR,GL_LINEAR,100.0f,100.0f,0.0f);
        scene->AddComponent<ColliderComponent>(m_Entities[2],100.0f,100.0f,0.0f,0.0f,0.0f,0.0f);
        scene->AddComponent<LightComponent>(m_Entities[2],0.0f,0.0f,200.0f,0.3f,Vec3(1,1,1),LIGHT_AROUND_POS);
        scene->GetComponent<LightComponent>(m_Entities[2])->SetCentered(100.0f,100.0f);

        m_SceneManager.SetCurrentScene("Test");

        RENDERER->SetAmbientLight({0.3,0.3,0.3});
        RENDERER->SetClearColor({0.3,0.3,0.3});
        RENDERER->ChangePointSize(100);
    }
    void OnUpdate(double frame_time) override{
        HandleButtons();
        Scene *scene=m_SceneManager.GetCurrentScene();
        
        RENDERER->Clear();
        scene->Update(frame_time);
        RENDERER->DrawLine(0,400,1600,400,{1,1,1,1},-2);
        RENDERER->DrawTriangle(300,300,500,300,700,700,{0,0,1,1},-1);
        RENDERER->DrawPoint(500,500,{1,0,0,1},-3);
        scene->Render();
    }

    void HandleButtons(){
        Scene *scene=m_SceneManager.GetCurrentScene();
        if(getKeyState(KEY_K,BUTTON_DOWN)){
            if(m_SceneManager.GetCurrentSceneName()=="Test")
                m_SceneManager.SetCurrentScene("Test2");
        }

        if(getKeyState(KEY_W,BUTTON_DOWN)){
            scene->MoveEntity(m_Entities[2],0.0f,1000.0f);
        }
        if(getKeyState(KEY_S,BUTTON_DOWN)){
            scene->MoveEntity(m_Entities[2],0.0f,-1000.0f);
        }
        if(getKeyState(KEY_A,BUTTON_DOWN)){
            scene->MoveEntity(m_Entities[2],-1000.0f,0.0f);
        }
        if(getKeyState(KEY_D,BUTTON_DOWN)){
            scene->MoveEntity(m_Entities[2],1000.0f,0.0f);
        }

        if(getKeyState(KEY_UP,BUTTON_DOWN)){
            scene->MoveEntity(m_Entities[0],0.0f,1000.0f);
        }
        if(getKeyState(KEY_DOWN,BUTTON_DOWN)){
            scene->MoveEntity(m_Entities[0],0.0f,-1000.0f);
        }
        if(getKeyState(KEY_LEFT,BUTTON_DOWN)){
            scene->MoveEntity(m_Entities[0],-1000.0f,0.0f);
        }
        if(getKeyState(KEY_RIGHT,BUTTON_DOWN)){
            scene->MoveEntity(m_Entities[0],1000.0f,0.0f);
        }

        if(getKeyState(KEY_P,BUTTON_DOWN)){
            if(scene->GetComponent<ColliderComponent>(m_Entities[0])!=nullptr)
                scene->RemoveComponent<ColliderComponent>(m_Entities[0]);
            else
                scene->AddComponent<ColliderComponent>(m_Entities[0],100.0f,100.0f,0.0f,0.0f,0.0f,0.0f);
        }
    }
private:
    std::vector<uint64_t> m_Entities;
};

int main(){
    Test *test=new Test("Test",1600,900);
    test->Run();
    delete test;

    DeinitGlfwWindow();

    return 0;
}