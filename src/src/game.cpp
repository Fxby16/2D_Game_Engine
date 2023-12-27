#include <game.hpp>

Game::Game(const char *window_name,unsigned int width,unsigned int height,bool imgui){
    m_WindowName=window_name;
    BASE_SCREEN_WIDTH=SCREEN_WIDTH=width;
    BASE_SCREEN_HEIGHT=SCREEN_HEIGHT=height;

    this->imgui=imgui;

    InitGlfwWindow(window_name);
    if(imgui)
        Renderer::ImGui_Init();

    #ifdef DEBUG
        last_time=glfwGetTime();
        PrintDebugInfo();
    #endif
}

Game::~Game(){
    if(imgui)
        Renderer::ImGui_Close();
    m_Entities.clear();
    DeinitGlfwWindow();
}

void Game::Run(){
    while(!glfwWindowShouldClose(WINDOW)){
        CURRENT_FRAME=glfwGetTime();
        DELTA_TIME=CURRENT_FRAME-LAST_FRAME;
        LAST_FRAME=CURRENT_FRAME;


        glfwPollEvents();

        RENDERER->StartScene();
        if(imgui){
            Renderer::ImGui_Start_Frame();
            Renderer::ImGui_Theme();
        }

        OnUpdate(DELTA_TIME);
        OnRender();
        if(FPS_COUNTER)
            ShowFpsCounter();

        if(imgui){
            OnImGuiUpdate();
            OnImGuiRender();
        }

        glfwSwapBuffers(WINDOW);

        #ifdef DEBUG
            double current_time=glfwGetTime();
            if(current_time-last_time>=4.0){
                last_time=current_time;
                printf("FPS: %f\n",1.0/(DELTA_TIME));
                PrintDebugInfo();
            }
        #endif
    }
}

void Game::OnUpdate(const double frame_time){
    RENDERER->Clear();
}

void Game::OnRender(){
    RENDERER->Render();
    RENDERER->ApplyLight();
    RENDERER->DrawScene();
}

void Game::OnImGuiUpdate(){
    Renderer::ImGui_Content();
}

void Game::OnImGuiRender(){
    Renderer::ImGui_End_Frame();
}