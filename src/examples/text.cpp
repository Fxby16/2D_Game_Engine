#include <game.hpp>

#include <vector>
#include <cmath>
#include <algorithm>
#include <textrenderer.hpp>
#include <string>
#include <memory.hpp>

class Example : public Game{
private:
    TextRenderer text_renderer;
    char *text;
    Vec3 color;
    float x,y,scale;
public:
    Example(const char *window_name,unsigned int width,unsigned int height,bool imgui=true): Game(window_name,width,height,imgui),text_renderer("resources/fonts/Tektur-Regular.ttf"),
        color(1,1,1),x(0),y(0),scale(1){

        m_Renderer->SetClearColor({0.5f,0.5f,0.5f});

        text=(char*)AllocateMemory(100*sizeof(char));
        memset(text,'\0',100*sizeof(char));
    }
    ~Example(){
        FreeMemory(text);
    }

    void OnUpdate(const double frame_time) override{
        m_Renderer->Clear();
        text_renderer.DrawText(text,x,y,scale,color);
    }

    void OnImGuiUpdate() override{
        Renderer::ImGui_Content();

        ImGui::SetNextWindowSize(ImVec2(0,0));
        ImGui::Begin("Text",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        ImGui::InputTextMultiline("String",text,100);
        ImGui::ColorPicker3("Color",(float *)&color.r);
        ImGui::SliderFloat("x",&x,0,SCREEN_WIDTH);
        ImGui::SliderFloat("y",&y,0,SCREEN_HEIGHT);
        ImGui::SliderFloat("Scale",&scale,0,100);
        ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH/2-ImGui::GetWindowWidth()/2,0));
        ImGui::End();
    }
};

int main(){
    Example *example=new Example("Test",1600,900);
    example->Run();
    delete example;

    return 0;
}