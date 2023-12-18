#include <game.hpp>
#include <cmath>

class Example : public Game{
private:
    SpriteSheet s;
public:
    Example(const char *window_name,unsigned int width,unsigned int height,bool imgui=true): Game(window_name,width,height,imgui),s("resources/textures/Run.png",128,128,GL_NEAREST,GL_NEAREST){
        s.PlayAnimation(true,0.1f);
    }
    ~Example(){}

    void OnUpdate(const double frame_time) override{
        m_Renderer->Clear();
        m_Renderer->DrawAnimatedTexture(300,300,300,300,0,s);
    }

    void OnImGuiUpdate() override{
        Renderer::ImGui_Content();
    }
};

int main(){
    Example *example=new Example("Test",1600,900);
    example->Run();
    delete example;

    return 0;
}