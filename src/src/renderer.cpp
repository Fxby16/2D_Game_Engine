#include <renderer.hpp>
#include <vector>
#include <algorithm>

#include <GLFW/glfw3.h>
#include <glfw.hpp>

Renderer::Renderer(): VB_T(MAX_VERTICES),VB_P(MAX_VERTICES),VB_L(MAX_VERTICES),buffer_T(MAX_VERTICES),
    buffer_P(MAX_VERTICES),buffer_L(MAX_VERTICES),
    shader_T("resources/shaders/textures/vertex.glsl","resources/shaders/textures/fragment.glsl"),
    shader_P("resources/shaders/points/vertex.glsl","resources/shaders/points/fragment.glsl"),
    shader_L("resources/shaders/lines/vertex.glsl","resources/shaders/lines/fragment.glsl"),
    shader_post_processing("resources/shaders/textures/vertex.glsl","resources/shaders/post_processing/fragment.glsl"),
    postprocessing_index(std::numeric_limits<unsigned int>::max()),Num_Vertices_T(0),Num_Vertices_P(0),Num_Vertices_L(0){

    framebuffer=new Framebuffer();

    proj=glm::ortho(0.0f,(float)SCREEN_WIDTH,0.0f,(float)SCREEN_HEIGHT,-1.0f,1.0f);
    for(int i=0;i<32;i++)
        slots[i]=i;

    Renderer::ImGui_Init(window);

    AddLayout(VBL_T,VA_T,GL_FLOAT,2,false);
    AddLayout(VBL_T,VA_T,GL_FLOAT,2,false);
    AddLayout(VBL_T,VA_T,GL_FLOAT,1,false);
    AddLayout(VBL_T,VA_T,GL_FLOAT,1,false);
    VA_T.AddBuffer(VB_T,VBL_T);

    AddLayout(VBL_P,VA_P,GL_FLOAT,2,false);
    AddLayout(VBL_P,VA_P,GL_FLOAT,4,false);
    VA_P.AddBuffer(VB_P,VBL_P);
    
    AddLayout(VBL_L,VA_L,GL_FLOAT,2,false);
    AddLayout(VBL_L,VA_L,GL_FLOAT,4,false);
    VA_L.AddBuffer(VB_L,VBL_L);

    shader_P.Bind();
    shader_P.SetUniformMat4f("u_PM",proj);

    shader_L.Bind();
    shader_L.SetUniformMat4f("u_PM",proj);

    shader_T.Bind();
    shader_T.SetUniformMat4f("u_PM",proj);
    shader_T.SetUniform1iv("texID",slots,32);

    shader_post_processing.Bind();
    shader_post_processing.SetUniformMat4f("u_PM",proj);
    shader_post_processing.SetUniform1iv("texID",slots,32);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glDepthMask(false);
    glPointSize(5);
    glLineWidth(5);

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&MaxTextureSlots);
}

Renderer::~Renderer(){
    delete framebuffer;
}

void Renderer::AddLayout(VertexBufferLayout &VBL,VertexArray &VA,unsigned int type,unsigned int count,bool normalized){
    VBL.Push(type,count,normalized);
}

void Renderer::RenderTexture(float x,float y,float w,float h,float scale,float depth,float texID){
    auto [a,b,c,d]=VertexBuffer::CreateQuad(x,y,w,h,scale,depth,texID);
    buffer_T[Num_Vertices_T]=a;
    buffer_T[Num_Vertices_T+1]=b;
    buffer_T[Num_Vertices_T+2]=c;
    buffer_T[Num_Vertices_T+3]=d;
    Num_Vertices_T+=4;

    if(Num_Vertices_T==MAX_VERTICES){ 
        Draw();
        Num_Vertices_T=0;
    }
}

void Renderer::RenderQuad(Vertex a,Vertex b,Vertex c,Vertex d){
    buffer_T[Num_Vertices_T]=a;
    buffer_T[Num_Vertices_T+1]=b;
    buffer_T[Num_Vertices_T+2]=c;
    buffer_T[Num_Vertices_T+3]=d;
    Num_Vertices_T+=4;

    if(Num_Vertices_T==MAX_VERTICES)
        Draw();
}

void Renderer::RenderPoint(float x,float y,float r,float g,float b,float a){
    buffer_P[Num_Vertices_P].pos={x,y};
    buffer_P[Num_Vertices_P].color={r,g,b,a};
    ++Num_Vertices_P;

    if(Num_Vertices_P==MAX_VERTICES)
        Draw();
}

void Renderer::RenderLine(float x1,float y1,float x2,float y2,float *color){
    buffer_L[Num_Vertices_L].pos={x1,y1};
    buffer_L[Num_Vertices_L].color={color[0],color[1],color[2],color[3]};
    buffer_L[Num_Vertices_L+1].pos={x2,y2};
    buffer_L[Num_Vertices_L+1].color=buffer_L[Num_Vertices_L].color;
    Num_Vertices_L+=2;

    if(Num_Vertices_L==MAX_VERTICES)
        Draw();
}

void Renderer::StartScene(){
    if(proj_update){
        proj_update=false;
        proj=glm::ortho(0.0f,(float)SCREEN_WIDTH,0.0f,(float)SCREEN_HEIGHT,-1.0f,1.0f);

        shader_P.Bind();
        shader_P.SetUniformMat4f("u_PM",proj);

        shader_L.Bind();
        shader_L.SetUniformMat4f("u_PM",proj);

        shader_T.Bind();
        shader_T.SetUniformMat4f("u_PM",proj);

        shader_post_processing.Bind();
        shader_post_processing.SetUniformMat4f("u_PM",proj);
    }
    if(framebuffer_update){
        framebuffer_update=false;
        delete framebuffer;
        framebuffer=new Framebuffer();
    }
    framebuffer->Bind();
    Clear();
}

void Renderer::DrawScene(){
    framebuffer->Unbind();
    Clear();
    RenderTexture(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,1,0,framebuffer->getColorbufferID());
    Draw(true);
}

void Renderer::Draw(bool finalScene){ //if this function gets called because there are MAX_VERTICES vertices, it's not guaranteed that it will respect depth input for subsequent vertices
    if(Num_Vertices_T==0 && Num_Vertices_P==0 && Num_Vertices_L==0)
        return;

int lastChecked=-1;
int slot=-1;
int lastIndex=0;

    if(Num_Vertices_T>0){ //draw textures
        std::stable_sort(begin(buffer_T),begin(buffer_T)+Num_Vertices_T,cmp); //sorting textures by id to reduce the times you have to bind new textures
        VA_T.Bind();
        VB_T.Bind();
        IB.Bind();

        if(postprocessing_index!=std::numeric_limits<unsigned int>::max() && finalScene)
            PostProcessing();
        else
            shader_T.Bind();

        for(int i=0;i<Num_Vertices_T;i++){
            if(buffer_T[i].texID!=lastChecked){ //new texture
                if(slot<MaxTextureSlots-1){ //slot available, change last id checked, increment slot and update the vertex
                    lastChecked=buffer_T[i].texID;
                    ++slot;
                    glActiveTexture(GL_TEXTURE0+slot);
                    glBindTexture(GL_TEXTURE_2D,lastChecked);
                    buffer_T[i].texID=(float)slot;
                }else{ //no slots available
                    VB_T.SetData(0,(float *)&buffer_T[lastIndex],i-lastIndex,sizeof(Vertex)); //send the data to the vertex buffer
                    IB.Set((i-lastIndex)); //send the indices to the index buffer
                    glDrawElements(GL_TRIANGLES,IB.GetNumElem(),GL_UNSIGNED_INT,nullptr); //draw
                    lastIndex=i; //update starting point for the next batch
                    lastChecked=buffer_T[i].texID;
                    slot=0;
                    buffer_T[i].texID=(float)slot;
                    DRAW_CALLS++;
                }
            }else{ //same texture, just update the vertex
                buffer_T[i].texID=(float)slot;
            }
        }
        if(Num_Vertices_T-lastIndex>0){ //if there are some vertices remaining, render them
            VB_T.SetData(0,(float *)&buffer_T[lastIndex],Num_Vertices_T-lastIndex,sizeof(Vertex));
            IB.Set((Num_Vertices_T-lastIndex));
            glDrawElements(GL_TRIANGLES,IB.GetNumElem(),GL_UNSIGNED_INT,nullptr);
            DRAW_CALLS++;
        }
    }

    if(Num_Vertices_P>0){
        VA_P.Bind();
        VB_P.Bind();

        if(postprocessing_index!=std::numeric_limits<unsigned int>::max() && finalScene)
            PostProcessing();
        else
            shader_P.Bind();

        VB_P.SetData(0,(float *)&buffer_P[0],Num_Vertices_P,sizeof(LinePointVertex));
        glDrawArrays(GL_POINTS,0,Num_Vertices_P);
    }

    if(Num_Vertices_L>0){
        VA_L.Bind();    
        VB_L.Bind();

        if(postprocessing_index!=std::numeric_limits<unsigned int>::max() && finalScene)
            PostProcessing();
        else
            shader_L.Bind();

        VB_L.SetData(0,(float *)&buffer_L[0],Num_Vertices_L,sizeof(LinePointVertex));
        glDrawArrays(GL_LINES,0,Num_Vertices_L);
    }

    Num_Vertices_T=Num_Vertices_P=Num_Vertices_L=0;
}

void Renderer::Clear() const{
    DRAW_CALLS=0;

    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::SetPostProcessing(const char *uniformName){
    if(uniformName==nullptr){
        postprocessing_index=std::numeric_limits<unsigned int>::max();
        return;
    }
    postprocessing_index=glGetSubroutineIndex(shader_post_processing.getID(),GL_FRAGMENT_SHADER,uniformName);;
}

void Renderer::PostProcessing(){
    shader_post_processing.Bind();
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER,1,&postprocessing_index);
}

void Renderer::ImGui_Init(GLFWwindow *window){
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    ImGui::GetIO().FontGlobalScale = 1.5f; // Increase the font size (adjust the scale as needed)
}

void Renderer::ImGui_Theme(){
ImVec4* colors=ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    //colors[ImGuiCol_DockingPreview]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    //colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding                     = ImVec2(8.00f, 8.00f);
    style.FramePadding                      = ImVec2(5.00f, 2.00f);
    style.CellPadding                       = ImVec2(6.00f, 6.00f);
    style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
    style.IndentSpacing                     = 25;
    style.ScrollbarSize                     = 15;
    style.GrabMinSize                       = 10;
    style.WindowBorderSize                  = 1;
    style.ChildBorderSize                   = 1;
    style.PopupBorderSize                   = 1;
    style.FrameBorderSize                   = 1;
    style.TabBorderSize                     = 1;
    //style.WindowRounding                    = 7;
    //style.ChildRounding                     = 4;
    //style.FrameRounding                     = 3;
    //style.PopupRounding                     = 4;
    //style.ScrollbarRounding                 = 9;
    //style.GrabRounding                      = 3;
    //style.LogSliderDeadzone                 = 4;
    //style.TabRounding                       = 4;
}

void Renderer::ImGui_Start_Frame(){
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void Renderer::ImGui_Content(){
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(ImVec2(0,0));
    ImGui::Begin("FPS",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
    currentFrame=glfwGetTime();
    deltaTime=currentFrame-lastFrame;
    lastFrame=currentFrame;

    FPS=1.0/deltaTime;
    ImGui::Text("FPS: %.1f",FPS);
    ImGui::Text("Frame Time: %.4f",deltaTime*1000);

    if(ImGui::Checkbox("V-Sync",&v_sync))
        glfwSwapInterval(v_sync);
    
    if(ImGui::Checkbox("Fullscreen",&WindowInfo::isFullScreen))
        ToggleFullScreen();
    ImGui::End();
}

void Renderer::ImGui_End_Frame(){
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::ImGui_Close(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}