#include <game.hpp>

Game::Game(const char *window_name){
    m_WindowName=window_name;

    InitGlfwWindow(window_name);
    Renderer::ImGui_Init();
    InitAudio();

    m_Renderer=new Renderer;
    m_AudioPlayer=new AudioPlayer;

    #ifdef DEBUG
        last_time=glfwGetTime();
        PrintDebugInfo();
    #endif
}

Game::~Game(){
    delete m_Renderer;
    delete m_AudioPlayer;

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

void Game::OnUpdate(double frame_time){
    m_Renderer->Clear();
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