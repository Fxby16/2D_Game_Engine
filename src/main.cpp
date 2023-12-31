#include <application.hpp>

class Test : public Application{
public:
    Test(const char *title,int width,int height,bool imgui=false): Application(title,width,height,imgui){
        m_SceneManager.AddScene("Test");
        m_SceneManager.SetCurrentScene("Test");
        Scene *scene=m_SceneManager.GetCurrentScene();

        m_Entities.push_back(scene->AddEntity());
        scene->AddComponent<TextureComponent>(m_Entities[0],"/home/fabio/Scaricati/Red.png",GL_LINEAR,GL_LINEAR,100.0f,100.0f,0.0f);
        scene->AddComponent<RigidbodyComponent>(m_Entities[0],RigidbodyComponent::BodyType::Dynamic,true);
        scene->AddComponent<BoxColliderComponent>(m_Entities[0],50.0f,50.0f,100.0f,100.0f,1.0f,0.0f,0.0f,0.5f);
        //scene->AddComponent<CircleColliderComponent>(m_Entities[0],50.0f,50.0f,50.0f,1.0f,0.0f,0.0f,0.5f);
        scene->AddComponent<LightComponent>(m_Entities[0],0.0f,0.0f,200.0f,0.3f,Vec3(1,1,1),LIGHT_AROUND_POS);
        scene->GetComponent<LightComponent>(m_Entities[0])->SetCentered(100.0f,100.0f);

        m_Entities.push_back(scene->AddEntity());
        scene->AddComponent<AnimatedTextureComponent>(m_Entities[1],"resources/textures/Run.png",(unsigned int)128,(unsigned int)128,GL_NEAREST,GL_NEAREST,300.0f,300.0f,0.0f);
        scene->AddComponent<RigidbodyComponent>(m_Entities[1],RigidbodyComponent::BodyType::Static,true);
        scene->AddComponent<BoxColliderComponent>(m_Entities[1],150.0f,150.0f,300.0f,300.0f,1.0f,0.0f,0.0f,0.5f);
        scene->SetEntityPosition(m_Entities[1],500,500);

        scene->OnPhysicsStart();
        scene->SetGravity(0.0f,0.0f);

        m_SceneManager.AddScene("Test2");
        m_SceneManager.SetCurrentScene("Test2");
        scene=m_SceneManager.GetCurrentScene();

        m_Entities.push_back(scene->AddEntity());
        scene->AddComponent<TextureComponent>(m_Entities[2],"/home/fabio/Scaricati/Red.png",GL_LINEAR,GL_LINEAR,100.0f,100.0f,0.0f);
        scene->AddComponent<RigidbodyComponent>(m_Entities[2],RigidbodyComponent::BodyType::Dynamic,true);
        scene->AddComponent<BoxColliderComponent>(m_Entities[2],50.0f,50.0f,100.0f,100.0f,1.0f,0.0f,0.0f,0.5f);
        scene->AddComponent<LightComponent>(m_Entities[2],0.0f,0.0f,200.0f,0.3f,Vec3(1,1,1),LIGHT_AROUND_POS);
        scene->GetComponent<LightComponent>(m_Entities[2])->SetCentered(100.0f,100.0f);

        scene->OnPhysicsStart();
        scene->SetGravity(0.0f,0.0f);

        m_SceneManager.SetCurrentScene("Test");

        RENDERER->SetAmbientLight({0.3,0.3,0.3});
        RENDERER->SetClearColor({0.3,0.3,0.3});
        RENDERER->ChangePointSize(100);
    }
    void OnUpdate(double frame_time) override{
        HandleButtons();
        Scene *scene=m_SceneManager.GetCurrentScene();
        
        scene->Update(frame_time);
    }

    void OnRender() override{
        RENDERER->Clear();
        
        RENDERER->DrawLine(0,400,1600,400,{1,1,1,1},-2);
        RENDERER->DrawTriangle(300,300,500,300,700,700,{0,0,1,1},-1);
        RENDERER->DrawPoint(500,500,{1,0,0,1},-3);

        Scene *scene=m_SceneManager.GetCurrentScene();
        scene->Render();
        
        RENDERER->Render();
        RENDERER->ApplyLight();
        RENDERER->DrawScene();
    }

    void HandleButtons(){
        Scene *scene=m_SceneManager.GetCurrentScene();
        if(getKeyState(KEY_K,BUTTON_DOWN)){
            m_SceneManager.RemoveScene("Test");
            m_SceneManager.SetCurrentScene("Test2");
        }

        Vec2 speed1,speed2;

        if(scene->GetName()=="Test"){
            if(getKeyState(KEY_UP,BUTTON_DOWN)){
                speed1.y+=1000.0f;
            }
            if(getKeyState(KEY_DOWN,BUTTON_DOWN)){
                speed1.y-=1000.0f;
            }
            if(getKeyState(KEY_LEFT,BUTTON_DOWN)){
                speed1.x-=1000.0f;
            }
            if(getKeyState(KEY_RIGHT,BUTTON_DOWN)){
                speed1.x+=1000.0f;
            }
            scene->MoveEntity(m_Entities[0],speed1.x,speed1.y);
        }else{
            if(getKeyState(KEY_W,BUTTON_DOWN)){
                speed2.y+=1000.0f;
            }
            if(getKeyState(KEY_S,BUTTON_DOWN)){
                speed2.y-=1000.0f;
            }
            if(getKeyState(KEY_A,BUTTON_DOWN)){
                speed2.x-=1000.0f;
            }
            if(getKeyState(KEY_D,BUTTON_DOWN)){
                speed2.x+=1000.0f;
            }
            scene->MoveEntity(m_Entities[2],speed2.x,speed2.y);
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