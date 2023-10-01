#include <renderer.hpp>
#include <examples.hpp>
#include <glfw.hpp>

int main(){
    if(InitGlfwWindow()<0)
        return -1;
{
Renderer renderer;
Texture texture("resources/textures/cicciogamer89.jpg",GL_LINEAR,GL_LINEAR);
SpriteSheet spritesheet("resources/textures/player.png",32,32,GL_NEAREST,GL_NEAREST);
    texture.Bind(0);
    spritesheet.Bind(1);

bool menus[2];
    memset(menus,0,sizeof(menus));

    while(!glfwWindowShouldClose(window)){
        renderer.Clear();
        Renderer::ImGui_Start_Frame();
        Renderer::ImGui_Performance();

        glfwPollEvents();

        if(menus[0])
            Examples::BatchRendering(renderer);
        else if(menus[1])
            Examples::Spritesheet(renderer,spritesheet);
    
        ImGui::SetNextWindowSize(ImVec2(0,0));
        ImGui::Begin("Menu",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        if(ImGui::BeginMenu("Examples")){
            if(ImGui::MenuItem("Batch Rendering")){
                memset(menus,0,sizeof(menus));
                menus[0]=true;
            }
            if(ImGui::MenuItem("SpriteSheet")){
                memset(menus,0,sizeof(menus));
                menus[1]=true;
            }
            if(ImGui::MenuItem("None"))
                memset(menus,0,sizeof(menus));
            ImGui::EndMenu();
        }
        ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH-ImGui::GetWindowWidth(),0));
        ImGui::End();

        Renderer::ImGui_End_Frame();

        glfwSwapBuffers(window);
    }

    Renderer::ImGui_Close();
}
    glfwTerminate();
    return 0;
}