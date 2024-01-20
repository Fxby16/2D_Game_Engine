#include <pch.hpp>
#include <editor.hpp>
#include <window.hpp>
#include <utilities.hpp>

Editor::Editor(unsigned int width,unsigned int height,float fullscreen_width,float fullscreen_height){
    Window::BaseWidth=Window::Width=width;
    Window::BaseHeight=Window::Height=height;
    Window::FullscreenWidth=fullscreen_width;
    Window::FullscreenHeight=fullscreen_height;
    Window::MAX_HEIGHT=Window::MAX_WIDTH/(Window::Width/Window::Height);

    Window::InitGlfwWindow("Editor");
    m_Camera.InitializeProj();
    m_SceneFramebuffer=new Framebuffer;
    RENDERER->ImGui_Init();
}

Editor::~Editor(){
    RENDERER->ImGui_Close();
}

void Editor::Run(){
    while(!glfwWindowShouldClose(Window::Window)){
        glfwPollEvents();

        RENDERER->StartEditorScene(this);
        RENDERER->DrawTriangle({2,2},{2,4},{4,4},{1,0,0,1},0);
        RENDERER->DrawEditorScene();
        
        RENDERER->StartScene();
        Renderer::ImGui_Start_Frame();
        Renderer::ImGui_Theme();
        
        OnImGuiUpdate();
        OnImGuiRender();
        RENDERER->DrawScene();

        glfwSwapBuffers(Window::Window);
    }
}

void Editor::OnSceneRender(){
}

void Editor::OnImGuiUpdate(){
    ImVec2 tempvec;
    float tempf;

    if(ImGui::BeginMainMenuBar()){
        tempvec=ImGui::GetWindowSize();
        if(ImGui::BeginMenu("File")){
            if(ImGui::MenuItem("New")){
            }
            if(ImGui::MenuItem("Open")){
                nfdchar_t *outPath=NULL;
                nfdresult_t result=NFD_OpenDialog(NULL,NULL,&outPath);
                    
                if(result==NFD_OKAY){
                    puts("Success!");
                    puts(outPath);
                    free(outPath);
                }
                else if(result==NFD_CANCEL){
                    puts("User pressed cancel.");
                }
                else{
                    printf("Error: %s\n",NFD_GetError());
                }
            }
            if(ImGui::MenuItem("Save")){
            }
            if(ImGui::MenuItem("Exit")){
                glfwSetWindowShouldClose(Window::Window, true);
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Edit")){
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Entities",nullptr,ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowPos(ImVec2(0,tempvec.y));
    ImGui::SetWindowSize(ImVec2(GetWidthPercentageInPx(20),Window::Height-tempvec.y));
    if(ImGui::TreeNodeEx("Node 1",ImGuiTreeNodeFlags_SpanAvailWidth)) {
        if(ImGui::TreeNodeEx("Leaf Node 1",ImGuiTreeNodeFlags_SpanAvailWidth)) {
            ImGui::Text("I am a leaf in the tree");
            ImGui::TreePop();
        }
        if(ImGui::TreeNodeEx("Leaf Node 2",ImGuiTreeNodeFlags_SpanAvailWidth)) {
            ImGui::Text("I am another leaf in the tree");
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    if(ImGui::TreeNodeEx("Node 2",ImGuiTreeNodeFlags_SpanAvailWidth)) {
        ImGui::Text("I am a childless node");
        ImGui::TreePop();
    }
    ImGui::End();

    ImGui::Begin("Viewport",nullptr,ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowPos(ImVec2(GetWidthPercentageInPx(20),tempvec.y));
    ImGui::SetWindowSize(ImVec2(0,0));
    tempf=GetWidthPercentageInPx(60);
    ImGui::Image((void*)m_SceneFramebuffer->GetColorbufferID(),ImVec2(tempf,tempf*9.0f/16.0f));
    ImGui::End();
}

void Editor::OnImGuiRender(){
    Renderer::ImGui_End_Frame();
}