#include <game.hpp>

class Test : public Game{
public:
    Test(const char *title,int width,int height,bool imgui=false): Game(title,width,height,imgui){
        m_Entities.resize(2);
        m_Entities[0].AddComponent<TextureComponent>("/home/fabio/Scaricati/Red.png",GL_LINEAR,GL_LINEAR,100.0f,100.0f,0);
        m_Entities[0].AddComponent<ColliderComponent>(100.0f,100.0f,0.0f,0.0f,0.0f,0.0f);
        m_Entities[0].AddComponent<LightComponent>(0.0f,0.0f,200.0f,0.3f,Vec3(1,1,1),LIGHT_AROUND_POS);
        CMLC.GetComponent(m_Entities[0].m_UID)->SetCentered(100.0f,100.0f);
        m_Entities[1].AddComponent<AnimatedTextureComponent>("resources/textures/Run.png",(unsigned int)128,(unsigned int)128,GL_NEAREST,GL_NEAREST,300.0f,300.0f,0);
        m_Entities[1].AddComponent<ColliderComponent>(300.0f,200.0f,0.0f,0.0f,0.0f,0.0f);
        m_Entities[1].SetPos(500,500);
        RENDERER->SetAmbientLight({0.3,0.3,0.3});
        RENDERER->SetClearColor({0.3,0.3,0.3});
        RENDERER->ChangePointSize(100);
    }
    void OnUpdate(double frame_time)override{
        RENDERER->Clear();
        HandleButtons();
        CMCC.Update(frame_time);
        RENDERER->DrawLine(0,400,1600,400,{1,1,1,1},-2);
        RENDERER->DrawTriangle(300,300,500,300,700,700,{0,0,1,1},-1);
        RENDERER->DrawPoint(500,500,{1,0,0,1},-3);
        CMTC.Render();
        CMATC.Render();
        CMCC.Render();
        CMLC.Render();
    }

    void HandleButtons(){
        if(getKeyState(KEY_W,BUTTON_DOWN)){
            m_Entities[0].Move(0.0f,1000.0f);
        }
        if(getKeyState(KEY_S,BUTTON_DOWN)){
            m_Entities[0].Move(0.0f,-1000.0f);
        }
        if(getKeyState(KEY_A,BUTTON_DOWN)){
            m_Entities[0].Move(-1000.0f,0.0f);
        }
        if(getKeyState(KEY_D,BUTTON_DOWN)){
            m_Entities[0].Move(1000.0f,0.0f);
        }
        if(getKeyState(KEY_P,BUTTON_DOWN)){
            if(m_Entities[0].GetComponent<ColliderComponent>()!=nullptr)    
                m_Entities[0].RemoveComponent<ColliderComponent>();
            else
                m_Entities[0].AddComponent<ColliderComponent>(100.0f,100.0f,0.0f,0.0f,0.0f,0.0f);
        }
    }
};

int main(){
    Test test("Test",1600,900);
    test.Run();

    return 0;
}