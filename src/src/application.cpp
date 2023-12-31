#include <application.hpp>

Application::Application(const char *window_name,unsigned int width,unsigned int height,bool imgui){
    m_WindowName=window_name;
    BASE_SCREEN_WIDTH=SCREEN_WIDTH=width;
    BASE_SCREEN_HEIGHT=SCREEN_HEIGHT=height;

    m_ImGui=imgui;

    InitGlfwWindow(window_name);
    if(imgui)
        Renderer::ImGui_Init();

    #ifdef DEBUG
        last_time=glfwGetTime();
        PrintDebugInfo();
    #endif
}

Application::~Application(){
    if(m_ImGui)
        Renderer::ImGui_Close();
}

void Application::Run(){
    while(!glfwWindowShouldClose(WINDOW)){
        glfwPollEvents();

        CURRENT_FRAME=glfwGetTime();
        DELTA_TIME=CURRENT_FRAME-LAST_FRAME;
        LAST_FRAME=CURRENT_FRAME;

        if(DELTA_TIME>0.25f)
            DELTA_TIME=0.25f;
            
        m_Accumulator+=DELTA_TIME;
        while(m_Accumulator>=m_FixedTimeStep){
            OnUpdate(m_FixedTimeStep);
            m_Accumulator-=m_FixedTimeStep;
        }

        ALPHA=m_Accumulator/m_FixedTimeStep;
    
        RENDERER->StartScene();
        
        if(m_ImGui){
            Renderer::ImGui_Start_Frame();
            Renderer::ImGui_Theme();
        }

        OnRender();
        
        if(FPS_COUNTER)
            ShowFpsCounter();

        if(m_ImGui){
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