#include <application.hpp>

Application::Application(const char *window_name,unsigned int width,unsigned int height,bool imgui){
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

Application::~Application(){
    if(imgui)
        Renderer::ImGui_Close();
}

void Application::Run(){
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

void Application::OnUpdate(const double frame_time){
    RENDERER->Clear();
}

void Application::OnRender(){
    RENDERER->Render();
    RENDERER->ApplyLight();
    RENDERER->DrawScene();
}

void Application::OnImGuiUpdate(){
    Renderer::ImGui_Content();
}

void Application::OnImGuiRender(){
    Renderer::ImGui_End_Frame();
}