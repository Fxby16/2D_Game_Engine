#include <game.hpp>

#include <vector>
#include <cmath>
#include <algorithm>

class Example : public Game{
private:
    std::vector<std::pair<Vec2,Vec2>>segments;
    bool show_points,show_lines,show_light;
    Texture ciccio;
    const char *type="CircleAroundPos";
public:
    Example(const char *window_name): Game(window_name),show_points(false),show_lines(false),show_light(true),ciccio("resources/textures/cicciogamer89.jpg",GL_LINEAR,GL_LINEAR){
        m_Renderer->ChangePointSize(3);
        m_Renderer->ChangeLineWidth(2);
        
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
        m_Renderer->Clear({0,0,0});

        m_Renderer->DrawTexture(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,1,0,ciccio.GetTexID());

        double xpos, ypos;
        GetMousePos(&xpos,&ypos);

        m_Renderer->UpdateScreenSegments();
        m_Renderer->DrawLight(static_cast<float>(xpos),static_cast<float>(ypos),type);
    }

    void OnImGuiUpdate() override{
        Renderer::ImGui_Content();

        ImGui::SetNextWindowSize(ImVec2(0,0));
        ImGui::Begin("Options",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        ImGui::Checkbox("Show Points",&show_points);
        ImGui::Checkbox("Show Lines",&show_lines);
        ImGui::Checkbox("Show Light",&show_light);
        ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH/2-ImGui::GetWindowWidth()/2,0));
        ImGui::End();

        ImGui::SetNextWindowSize(ImVec2(0,0));
        ImGui::Begin("Type",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        if(ImGui::BeginMenu("Type")){
            if(ImGui::MenuItem("CircleAroundPos"))
                type="CircleAroundPos";
            if(ImGui::MenuItem("CircleAroundPosColl"))
                type="CircleAroundPosColl";
            if(ImGui::MenuItem("AllLight"))
                type="AllLight";
            ImGui::EndMenu();
        }
        ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH-ImGui::GetWindowWidth(),0));
        ImGui::End();
    }
};

int main(){
    Example *example=new Example("Test");
    example->Run();
    delete example;

    return 0;
}