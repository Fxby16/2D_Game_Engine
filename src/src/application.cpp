#include <application.hpp>

Application::Application(const char *window_name,unsigned int width,unsigned int height,bool imgui){
    m_WindowName=window_name;
    Window::BaseWidth=Window::Width=width;
    Window::BaseHeight=Window::Height=height;
    Window::MAX_HEIGHT=Window::MAX_WIDTH/(Window::Width/Window::Height);

    m_ImGui=imgui;

    Window::InitGlfwWindow(window_name);
    if(imgui)
        Renderer::ImGui_Init();

    #ifdef DEBUG
        last_time=glfwGetTime();
        Window::PrintDebugInfo();
    #endif
}

Application::~Application(){
    if(m_ImGui)
        Renderer::ImGui_Close();
}

void Application::Run(){
    while(!glfwWindowShouldClose(Window::Window)){
        glfwPollEvents();

        Window::CurrentFrameTime=glfwGetTime();
        Window::DeltaTime=Window::CurrentFrameTime-Window::LastFrameTime;
        Window::LastFrameTime=Window::CurrentFrameTime;

        if(Window::DeltaTime>0.25f)
            Window::DeltaTime=0.25f;
            
        m_Accumulator+=Window::DeltaTime;
        while(m_Accumulator>=m_FixedTimeStep){
            OnUpdate(m_FixedTimeStep);
            m_Accumulator-=m_FixedTimeStep;
        }

        Window::Alpha=m_Accumulator/m_FixedTimeStep;
    
        RENDERER->StartScene();
        
        if(m_ImGui){
            Renderer::ImGui_Start_Frame();
            Renderer::ImGui_Theme();
        }

        OnRender();
        
        if(Window::ShowFpsCounter_)
            Window::ShowFpsCounter();

        if(m_ImGui){
            OnImGuiUpdate();
            OnImGuiRender();
        }

        glfwSwapBuffers(Window::Window);

        #ifdef DEBUG
            double current_time=glfwGetTime();
            if(current_time-last_time>=4.0){
                last_time=current_time;
                printf("FPS: %f\n",1.0/Window::DeltaTime);
                Window::PrintDebugInfo();
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