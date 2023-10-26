#include <renderer.hpp>
#include <textrenderer.hpp>
#include <examples.hpp>
#include <glfw.hpp>
#include <framebuffer.hpp>

int main(){
    if(InitGlfwWindow()<0)
        return -1;

    ImGui::CreateContext();
{
Renderer renderer;
TextRenderer textrenderer("resources/fonts/Tektur-Regular.ttf");
Texture texture("resources/textures/cicciogamer89.jpg",GL_LINEAR,GL_LINEAR);
Texture texture2("resources/textures/smurf_cat.jpg",GL_LINEAR,GL_LINEAR);
SpriteSheet spritesheet("resources/textures/spritesheet.png",32,32,GL_NEAREST,GL_NEAREST);

std::vector<Texture*>t(457);
    for(int i=0;i<t.size();i++) //using new because when it copies the object, it calls the destructor and delete the texture. will be manually deleted at the end
        t[i]=new Texture("resources/textures/batching_multiple_textures/"+std::to_string(i)+".png",GL_NEAREST,GL_NEAREST);

bool menus[7];
    memset(menus,0,sizeof(menus));

    while(!glfwWindowShouldClose(window)){
        HandleInputs();
        renderer.StartScene();
        
        Renderer::ImGui_Start_Frame();
        Renderer::ImGui_Theme();
        Renderer::ImGui_Content();

        glfwPollEvents();

        if(menus[0])
            Examples::BatchRendering(renderer,texture);
        else if(menus[1])
            Examples::Spritesheet(renderer,spritesheet);
        else if(menus[2])
            Examples::BatchingMultipleTextures(renderer,t);
        else if(menus[3])
            Examples::DepthTest(renderer,*t[0],*t[10]);
        else if(menus[4])
            Examples::LinesPoints(renderer);
        else if(menus[5])
            Examples::PostProcessing(renderer,texture2);
        else if(menus[6])
            Examples::Text(textrenderer);
    
        ImGui::SetNextWindowSize(ImVec2(0,0));
        ImGui::Begin("Menu",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        if(ImGui::BeginMenu("Examples")){
            if(ImGui::MenuItem("Batch Rendering")){
                memset(menus,0,sizeof(menus));
                menus[0]=true;
            }
            else if(ImGui::MenuItem("SpriteSheet")){
                memset(menus,0,sizeof(menus));
                menus[1]=true;
            }
            else if(ImGui::MenuItem("Batching Multiple Textures")){
                memset(menus,0,sizeof(menus));
                menus[2]=true;
            }
            else if(ImGui::MenuItem("Depth")){
                memset(menus,0,sizeof(menus));
                menus[3]=true;
            }
            else if(ImGui::MenuItem("Lines/Points")){
                memset(menus,0,sizeof(menus));
                menus[4]=true;
            }
            else if(ImGui::MenuItem("Post Processing")){
                memset(menus,0,sizeof(menus));
                menus[5]=true;
            }
            else if(ImGui::MenuItem("Text")){
                memset(menus,0,sizeof(menus));
                menus[6]=true;
            }
            else if(ImGui::MenuItem("None"))
                memset(menus,0,sizeof(menus));
            ImGui::EndMenu();
        }
        ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH-ImGui::GetWindowWidth(),0));
        ImGui::End();

        renderer.DrawScene();
        Renderer::ImGui_End_Frame();
        glfwSwapBuffers(window);
    }

    Renderer::ImGui_Close();
    for(int i=0;i<40;i++)
        delete t[i];
}
    glfwTerminate();
    return 0;
}