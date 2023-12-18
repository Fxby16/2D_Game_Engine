#include <game.hpp>

#include <vector>
#include <cmath>
#include <algorithm>

class Example : public Game{
private:
    std::vector<std::pair<Vec2,Vec2>>segments;
    Texture img;
    float light_radius;
    enum LightType type;
public:
    Example(const char *window_name,unsigned int width,unsigned int height,bool imgui=true): Game(window_name,width,height,imgui),img("resources/textures/cicciogamer89.jpg",GL_LINEAR,GL_LINEAR),light_radius(300.0f),type(LIGHT_AROUND_POS_COLL){
        m_Renderer->ChangePointSize(20);
        m_Renderer->ChangeLineWidth(2);

        m_Renderer->SetAmbientLight({0.3f,0.3f,0.3f});
        
        m_Renderer->AddSegment(Vec2(300,200),Vec2(700,200));
        m_Renderer->AddSegment(Vec2(700,200),Vec2(700,500));
        m_Renderer->AddSegment(Vec2(700,500),Vec2(300,500));
        m_Renderer->AddSegment(Vec2(300,500),Vec2(300,200));

        m_Renderer->AddSegment(Vec2(100,200),Vec2(200,200));
        m_Renderer->AddSegment(Vec2(200,200),Vec2(200,300));
        m_Renderer->AddSegment(Vec2(200,300),Vec2(100,300));
        m_Renderer->AddSegment(Vec2(100,300),Vec2(100,200));

        m_Renderer->AddSegment(Vec2(1100,600),Vec2(1100,100));
        m_Renderer->AddSegment(Vec2(1100,100),Vec2(1000,100));
        m_Renderer->AddSegment(Vec2(1000,100),Vec2(1000,600));
        m_Renderer->AddSegment(Vec2(1000,600),Vec2(1100,600));
    }

    void OnUpdate(double frame_time) override{
        m_Renderer->Clear();
        m_Renderer->DrawTexture(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0,img.GetTexID());
        m_Renderer->DrawSolidQuad(300.0f,200.0f,400.0f,300.0f,{0,0,0,1});
        m_Renderer->DrawSolidQuad(100.0f,200.0f,100.0f,100.0f,{0,0,0,1});
        m_Renderer->DrawSolidQuad(1000.0f,100.0f,100.0f,500.0f,{0,0,0,1});

        double xpos, ypos;
        GetMousePos(&xpos,&ypos);

        m_Renderer->Render();

        m_Renderer->UpdateScreenSegments();
        m_Renderer->DrawLight(xpos,ypos,{1.0f,1.0f,1.0f,1.0f},type,light_radius,0.8f);
        m_Renderer->DrawLight(180.0f,600.0f,{0.3f,1.0f,0.3f,1.0f},type,100.0f,0.7f);
        m_Renderer->DrawLight(1400.0f,700.0f,{1.0f,0.0f,0.0f,1.0f},type,200.0f,0.7f);
        m_Renderer->DrawLight(800.0f,450.0f,{0.0f,0.0f,1.0f,1.0f},type,200.0f,0.7f);
        m_Renderer->ApplyLight();

        m_Renderer->DrawPoint(xpos,ypos,0.0f,1.0f,1.0f,1.0f);
        m_Renderer->DrawPoint(180.0f,600.0f,0.0f,1.0f,1.0f,1.0f);
        m_Renderer->DrawPoint(1400.0f,700.0f,0.0f,1.0f,1.0f,1.0f);
        m_Renderer->DrawPoint(800.0f,450.0f,0.0f,1.0f,1.0f,1.0f);
        m_Renderer->Render();
    }

    void OnImGuiUpdate() override{
        Renderer::ImGui_Content();

        ImGui::SetNextWindowSize(ImVec2(0,0));
        ImGui::Begin("Light Radius",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        ImGui::SliderFloat("Radius",&light_radius,0.0f,500.0f);
        ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH/2-ImGui::GetWindowWidth()/2,0));
        ImGui::End();

        ImGui::SetNextWindowSize(ImVec2(0,0));
        ImGui::Begin("Type",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        if(ImGui::BeginMenu("Type")){
            if(ImGui::MenuItem("LightAroundPos"))
                type=LIGHT_AROUND_POS;
            if(ImGui::MenuItem("AllLight"))
                type=ALL_LIGHT;
            if(ImGui::MenuItem("LightAroundPosColl"))
                type=LIGHT_AROUND_POS_COLL;
            ImGui::EndMenu();
        }
        ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH-ImGui::GetWindowWidth(),0));
        ImGui::End();
    }
};

int main(){
    Example *example=new Example("Test",1600,900);
    example->Run();
    delete example;

    return 0;
}