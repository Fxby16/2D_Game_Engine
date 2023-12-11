#include <game.hpp>
#include <cmath>

class Example : public Game{
private:
    SpriteSheet s;
public:
    Example(const char *window_name): Game(window_name),s("resources/textures/Run.png",128,128,GL_NEAREST,GL_NEAREST){
        s.PlayAnimation(true,0.1f);
    }
    ~Example(){}

    void OnUpdate(double frame_time) override{
        m_Renderer->Clear({0.0f,0.0f,0.0f});
        m_Renderer->DrawAnimatedTexture(300,300,300,300,0,s);
    }

    void OnImGuiUpdate() override{
        Renderer::ImGui_Content();
    }
};

int main(){
    Example *example=new Example("Test");
    example->Run();
    delete example;

    return 0;
}