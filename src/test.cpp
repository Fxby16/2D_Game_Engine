#include <application.hpp>
#include <buttons.hpp>

#include "test.hpp"

using namespace Window;

class Test : public Application{
public:
    Test(const char *window_name,unsigned int width,unsigned int height,float fullscreen_width,float fullscreen_height,bool imgui=false): Application(window_name,width,height,fullscreen_width,fullscreen_height,imgui){
        m_SceneManager.AddScene("Test");
        m_SceneManager.SetCurrentScene("Test");
        Scene *scene=m_SceneManager.GetCurrentScene();

        scene->OnPhysicsStart();

        m_Texture=std::make_shared<Texture>("/home/fabio/Scaricati/Red.png",GL_LINEAR,GL_LINEAR);

        m_Entities.push_back(scene->AddEntity());
        scene->AddComponent<TextureComponent>(m_Entities[0],m_Texture,1.0f,1.0f,0.0f);
        scene->AddComponent<RigidbodyComponent>(m_Entities[0],RigidbodyComponent::BodyType::Dynamic,true);
        scene->AddComponent<BoxColliderComponent>(m_Entities[0],0.5f,0.5f,1.0f,1.0f,1.0f,0.0f,0.0f,0.5f);
        //scene->AddComponent<CircleColliderComponent>(m_Entities[0],0.5f,0.5f,1.0f,1.0f,0.0f,0.0f,0.5f);
        scene->AddComponent<LightComponent>(m_Entities[0],0.0f,0.0f,2.0f,0.3f,Vec3(1,1,1),LIGHT_AROUND_POS_COLL);
        scene->GetComponent<LightComponent>(m_Entities[0])->SetCentered(1.0f,1.0f);
        scene->SetEntityPosition(m_Entities[0],0.0f,0.0f);
        scene->AddComponent<NativeScriptComponent>(m_Entities[0]);

        auto nsc=scene->GetComponent<NativeScriptComponent>(m_Entities[0]);
        nsc->OnUpdate=Entity1Update;

        m_Entities.push_back(scene->AddEntity());
        scene->AddComponent<AnimatedTextureComponent>(m_Entities[1],"resources/textures/Run.png",128,128,GL_NEAREST,GL_NEAREST,3.0f,3.0f,0.0f);
        scene->AddComponent<RigidbodyComponent>(m_Entities[1],RigidbodyComponent::BodyType::Static,true);
        //scene->AddComponent<BoxColliderComponent>(m_Entities[1],1.5f,1.5f,3.0f,3.0f,1.0f,0.0f,0.0f,0.5f);
        scene->AddComponent<CircleColliderComponent>(m_Entities[1],1.5f,1.5f,1.0f,1.0f,0.0f,0.0f,0.5f);
        scene->AddComponent<LightComponent>(m_Entities[1],0.0f,0.0f,2.0f,0.3f,Vec3(1,1,1),LIGHT_AROUND_POS_COLL);
        scene->GetComponent<LightComponent>(m_Entities[1])->SetCentered(3.0f,3.0f);
        scene->SetEntityPosition(m_Entities[1],7.5f,4.0f);


        m_SceneManager.AddScene("Test2");
        m_SceneManager.SetCurrentScene("Test2");
        scene=m_SceneManager.GetCurrentScene();
        
        scene->OnPhysicsStart();
        scene->SetGravity(0.0f,0.0f);

        m_Entities.push_back(scene->AddEntity());
        scene->AddComponent<TextureComponent>(m_Entities[2],m_Texture,1.0f,1.0f,0.0f);
        scene->AddComponent<RigidbodyComponent>(m_Entities[2],RigidbodyComponent::BodyType::Dynamic,true);
        scene->AddComponent<BoxColliderComponent>(m_Entities[2],0.5f,0.5f,1.0f,1.0f,1.0f,0.0f,0.0f,0.5f);
        scene->AddComponent<LightComponent>(m_Entities[2],0.0f,0.0f,2.0f,0.3f,Vec3(1,1,1),LIGHT_AROUND_POS);
        scene->GetComponent<LightComponent>(m_Entities[2])->SetCentered(1.0f,1.0f);
        scene->AddComponent<NativeScriptComponent>(m_Entities[2]);

        nsc=scene->GetComponent<NativeScriptComponent>(m_Entities[2]);
        nsc->OnUpdate=Entity2Update;


        m_SceneManager.SetCurrentScene("Test");

        RENDERER->SetAmbientLight({0.3,0.3,0.3});
        RENDERER->SetClearColor({0.3,0.3,0.3});
        RENDERER->SetLineWidth(0.1f);
        RENDERER->SetPointSize(2.0f);

        RENDERER->AddSegment({5,3},{5,4});
        RENDERER->AddSegment({5,3},{6,3});
    }

    void OnUpdate(double frame_time) override{
        HandleButtons();
        Scene *scene=m_SceneManager.GetCurrentScene();
        scene->Update(frame_time);
    }

    void OnRender() override{
        RENDERER->DrawLine({0,MAX_HEIGHT/2.0f},{MAX_WIDTH,MAX_HEIGHT/2.0f},{1,1,1,1},-2);
        RENDERER->DrawTriangle({6.0f,1.0f},{7.0f,1.0f},{6.5f,2.0f},{0,0,1,1},-1);
        RENDERER->DrawPoint({4.0f,4.0f},{1,0,0,1},-3);

        Scene *scene=m_SceneManager.GetCurrentScene();
        scene->Render();
        
        RENDERER->Render();
        RENDERER->ApplyLight();
        #ifdef DEBUG
            scene->DebugDraw();
        #endif
        m_Camera.DrawSceneProj();
        RENDERER->DrawScene();
        m_Camera.ResetSceneProj();
    }

    void HandleButtons(){
        Scene *scene=m_SceneManager.GetCurrentScene();
        if(GetKeyState(KEY_K,BUTTON_DOWN)){
            m_SceneManager.RemoveScene("Test");
            m_SceneManager.SetCurrentScene("Test2");
        }

        if(GetKeyState(KEY_O,BUTTON_DOWN)){
            m_Camera.Zoom(0.1f);
        }
        if(GetKeyState(KEY_P,BUTTON_DOWN)){
            m_Camera.Zoom(-0.1f);
        }
        if(GetKeyState(KEY_U,BUTTON_DOWN)){
            m_Camera.Move(0.5f,0.0f);
        }
        if(GetKeyState(KEY_I,BUTTON_DOWN)){
            m_Camera.Move(-0.5f,0.0f);
        }
        if(GetKeyState(KEY_T,BUTTON_DOWN)){
            m_Camera.Rotate(1.0f);
        }
        if(GetKeyState(KEY_Y,BUTTON_DOWN)){
            m_Camera.Rotate(-1.0f);
        }
    }
private:
    std::vector<uint32_t> m_Entities;
    std::shared_ptr<Texture>m_Texture;
};

Application* Window::CreateApplication(){
    return new Test("Test",1600,900,1920,1080);
}