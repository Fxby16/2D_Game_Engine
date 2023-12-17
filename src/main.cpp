#include <game.hpp>

class Example : public Game{
private:
    SpriteSheet s;
    TextRenderer tr;
    double last_time=0;
    float fps;
public:
    Example(const char *window_name,unsigned int width,unsigned int height,bool imgui=true): Game(window_name,width,height,imgui),
        s("resources/textures/Run.png",128,128,GL_NEAREST,GL_NEAREST),tr("resources/fonts/Tektur-Regular.ttf"){
        ToggleVSync();
        last_time=-1.0;
        s.PlayAnimation(true,0.1f);
    }
    ~Example(){}

    void OnUpdate(double frame_time) override{
        m_Renderer->Clear();
        m_Renderer->DrawAnimatedTexture(300,300,300,300,0,s);
        m_Renderer->Render();

        double current_time=glfwGetTime();
        if(current_time-last_time>=0.5){
            last_time=current_time;
            fps=1.0f/frame_time;
        }
        tr.DrawText("FPS: "+std::to_string((int)fps),0,SCREEN_HEIGHT-tr.GetTextSize("FPS: "+std::to_string((int)fps),0.5f).second,0.5f,Vec3(1.0f,1.0f,1.0f));
    }
};

int main(){
    Example *example=new Example("Test",1280,720,false);
    example->Run();
    delete example;

    return 0;
}