#include <game.hpp>

class Example : public Game{
private:
    std::vector<Texture*>t;
    unsigned int IMAGE_WIDTH,IMAGE_HEIGHT,NUM_QUADS;
public:
    Example(const char *window_name): Game(window_name),t(457),IMAGE_WIDTH(20),IMAGE_HEIGHT(20){
        for(size_t i=0;i<t.size();i++){
            t[i]=new Texture("resources/textures/batching_multiple_textures/"+std::to_string(i)+".png",GL_NEAREST,GL_NEAREST);
        }
    }

    ~Example(){
        for(size_t i=0;i<t.size();i++)
            delete t[i];
    }

    void OnUpdate(double frame_time) override{
        NUM_QUADS=(unsigned int)ceil(((float)SCREEN_WIDTH/(float)IMAGE_WIDTH)*((float)SCREEN_HEIGHT/(float)IMAGE_HEIGHT));

        m_Renderer->Clear({0.0f,0.0f,0.0f});

        float x=0.0f,y=0.0f;
        for(unsigned int i=0;i<NUM_QUADS;i++){
            m_Renderer->DrawTexture(x,y,(float)IMAGE_WIDTH,(float)IMAGE_HEIGHT,1.0f,1.0f,(float)t[i%t.size()]->GetTexID());
            x+=IMAGE_WIDTH;
            if(x>=SCREEN_WIDTH){
                x=0;
                y+=IMAGE_HEIGHT;
            }
            if(y>=SCREEN_HEIGHT){
                y=0;
                x=0;
            }
        }
    }

    void OnImGuiUpdate() override{
        Renderer::ImGui_Content();

        ImGui::SetNextWindowSize(ImVec2(0,0));
        ImGui::Begin("Batching Multiple Textures",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::Text("NUM QUADS: %d",NUM_QUADS);
        ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH/2-ImGui::GetWindowWidth()/2,0));
        ImGui::End();
    }
};

int main(){
    Example *example=new Example("Test");
    example->Run();
    delete example;

    return 0;
}