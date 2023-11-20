#include <game.hpp>

Game::Game(const char *window_name){
    m_WindowName=window_name;

    InitGlfwWindow(window_name);
    InitAudio();
    ImGui::CreateContext();

    m_Renderer=new Renderer;
}

Game::~Game(){
    delete m_Renderer;

    DeinitAudio();
    Renderer::ImGui_Close();
    glfwTerminate();
}

void Game::Run(){
    while(!glfwWindowShouldClose(WINDOW)){
        CURRENT_FRAME=glfwGetTime();
        DELTA_TIME=CURRENT_FRAME-LAST_FRAME;
        LAST_FRAME=CURRENT_FRAME;

        HandleInputs();
        glfwPollEvents();

        m_Renderer->StartScene();
        Renderer::ImGui_Start_Frame();
        Renderer::ImGui_Theme();

        OnUpdate(DELTA_TIME);
        OnImGuiUpdate();

        OnRender();
        OnImGuiRender();

        glfwSwapBuffers(WINDOW);
    }
}

void Game::OnUpdate(double frame_time){
    m_Renderer->Clear({0.0f,0.0f,0.0f});
}

void Game::OnRender(){
    m_Renderer->Render();
    m_Renderer->DrawScene();
}

void Game::OnImGuiUpdate(){
    Renderer::ImGui_Content();
}

void Game::OnImGuiRender(){
    Renderer::ImGui_End_Frame();
}