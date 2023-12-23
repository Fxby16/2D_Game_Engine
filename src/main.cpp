#include <game.hpp>

class Test : public Game{
public:
    Test(const char *title,int width,int height,bool imgui=false): Game(title,width,height,imgui){
        e1.AddComponent<TextureComponent>("/home/fabio/Scaricati/Red.png",GL_LINEAR,GL_LINEAR,100.0f,100.0f,0);
        e1.AddComponent<ColliderComponent>(100.0f,100.0f,0.0f,0.0f,0.0f,0.0f);
        e2.AddComponent<AnimatedTextureComponent>("resources/textures/Run.png",(unsigned int)128,(unsigned int)128,GL_NEAREST,GL_NEAREST,300.0f,300.0f,0);
        e2.AddComponent<ColliderComponent>(300.0f,200.0f,0.0f,0.0f,0.0f,0.0f);
        e2.SetPos(500,500);
    }
    void OnUpdate(double frame_time)override{
        m_Renderer->Clear();
        HandleButtons();
        CMCC.Update(frame_time);
        CMTC.Render();
        CMATC.Render();
        CMCC.Render();
    }

    void HandleButtons(){
        if(getKeyState(KEY_W,BUTTON_DOWN)){
            e1.Move(0.0f,1000.0f);
        }
        if(getKeyState(KEY_S,BUTTON_DOWN)){
            e1.Move(0.0f,-1000.0f);
        }
        if(getKeyState(KEY_A,BUTTON_DOWN)){
            e1.Move(-1000.0f,0.0f);
        }
        if(getKeyState(KEY_D,BUTTON_DOWN)){
            e1.Move(1000.0f,0.0f);
        }
        if(getKeyState(KEY_P,BUTTON_DOWN)){
            if(e1.GetComponent<ColliderComponent>()!=nullptr)    
                e1.RemoveComponent<ColliderComponent>();
            else
                e1.AddComponent<ColliderComponent>(100.0f,100.0f,0.0f,0.0f,0.0f,0.0f);
        }
    }
private:
    Entity e1;
    Entity e2;
};

int main(){
    Test test("Test",1600,900);
    test.Run();

    return 0;
}