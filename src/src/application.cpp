#include <application.hpp>
#include <Instrumentor.h>

Application::Application(const char *window_name,unsigned int width,unsigned int height,float fullscreen_width,float fullscreen_height,bool imgui){
    m_WindowName=window_name;
    Window::BaseWidth=Window::Width=width;
    Window::BaseHeight=Window::Height=height;
    Window::FullscreenWidth=fullscreen_width;
    Window::FullscreenHeight=fullscreen_height;
    Window::MAX_HEIGHT=Window::MAX_WIDTH/(Window::Width/Window::Height);

    m_ImGui=imgui;

    Window::InitGlfwWindow(window_name);
    m_Camera.InitializeProj();
    if(imgui)
        Renderer::ImGui_Init();

    #ifdef DEBUG
        last_time=glfwGetTime();
        Window::PrintDebugInfo();
    #endif

    #ifdef ENABLE_PROFILING
        Instrumentor::Get().BeginSession("Profile");
    #endif
}

Application::~Application(){
    if(m_ImGui)
        Renderer::ImGui_Close();
    #ifdef ENABLE_PROFILING
        Instrumentor::Get().EndSession();
    #endif
}

void Application::Run(){
    PROFILE_FUNCTION();

    while(!glfwWindowShouldClose(Window::Window)){
        glfwPollEvents();

        if(Window::ProjUpdate){
            m_Camera.InitializeProj();
            Window::ProjUpdate=false;
        }

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
        
        if(Window::ShowMetrics_)
            Window::ShowMetrics();

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
    PROFILE_FUNCTION();

    RENDERER->Clear();
}

void Application::OnRender(){
    PROFILE_FUNCTION();
    
    RENDERER->Render();
    RENDERER->ApplyLight();
    RENDERER->DrawScene();
}

void Application::OnImGuiUpdate(){
    PROFILE_FUNCTION();
    
    Renderer::ImGui_Content();
}

void Application::OnImGuiRender(){
    PROFILE_FUNCTION();
    
    Renderer::ImGui_End_Frame();
}