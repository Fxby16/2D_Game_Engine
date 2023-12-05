#include <nfd.h>
#include <cstdio>
#include <cstdlib>
#include <window.hpp>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <global.hpp>

int main(){
    init();

    while(!glfwWindowShouldClose(WINDOW)){
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_StartFrame();

        if(ImGui::BeginMainMenuBar()){
            if(ImGui::BeginMenu("File")){
                if(ImGui::MenuItem("New")){
                }
                if(ImGui::MenuItem("Open")){
                    nfdchar_t *outPath=NULL;
                    nfdresult_t result=NFD_OpenDialog(NULL, NULL,&outPath);
                        
                    if(result==NFD_OKAY){
                        puts("Success!");
                        puts(outPath);
                        free(outPath);
                    }
                    else if(result==NFD_CANCEL){
                        puts("User pressed cancel.");
                    }
                    else{
                        printf("Error: %s\n", NFD_GetError() );
                    }
                }
                if(ImGui::MenuItem("Save")){
                }
                if(ImGui::MenuItem("Exit")){
                    glfwSetWindowShouldClose(WINDOW, true);
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Edit")){
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui_EndFrame();
        glfwPollEvents();
        glfwSwapBuffers(WINDOW);
    }

    deinit();

    return 0;
}