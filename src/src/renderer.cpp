#include <renderer.hpp>
#include <vector>
#include <algorithm>

#include <GLFW/glfw3.h>
#include <glfw.hpp>

RendererData::RendererData(const char *vertex_path,const char *fragment_path,unsigned int vertex_size): 
                          VBO(MAX_VERTICES,vertex_size,GL_DYNAMIC_DRAW),S(vertex_path,fragment_path),NumVertices(0){}

Renderer::Renderer(): 
    m_T("resources/shaders/textures/vertex.glsl","resources/shaders/textures/fragment.glsl",sizeof(Vertex)),
    m_P("resources/shaders/points/vertex.glsl","resources/shaders/points/fragment.glsl",sizeof(LinePointVertex)),
    m_L("resources/shaders/lines/vertex.glsl","resources/shaders/lines/fragment.glsl",sizeof(LinePointVertex)),
    m_BufferT(MAX_VERTICES),m_BufferP(MAX_VERTICES),m_BufferL(MAX_VERTICES),
    m_SPostProcessing("resources/shaders/textures/vertex.glsl","resources/shaders/post_processing/fragment.glsl"),
    m_PostProcessingIndex(std::numeric_limits<unsigned int>::max()){

    m_Framebuffer=new Framebuffer();
    IB.Set(MAX_VERTICES);

    m_Proj=glm::ortho(0.0f,(float)SCREEN_WIDTH,0.0f,(float)SCREEN_HEIGHT,-1.0f,1.0f);
    for(int i=0;i<32;i++)
        m_Slots[i]=i;

    Renderer::ImGui_Init();

    AddLayout(m_T.VBL,GL_FLOAT,2,false);
    AddLayout(m_T.VBL,GL_FLOAT,2,false);
    AddLayout(m_T.VBL,GL_FLOAT,1,false);
    AddLayout(m_T.VBL,GL_FLOAT,1,false);
    m_T.VAO.AddBuffer(m_T.VBO,m_T.VBL);

    AddLayout(m_P.VBL,GL_FLOAT,2,false);
    AddLayout(m_P.VBL,GL_FLOAT,4,false);
    m_P.VAO.AddBuffer(m_P.VBO,m_P.VBL);
    
    AddLayout(m_L.VBL,GL_FLOAT,2,false);
    AddLayout(m_L.VBL,GL_FLOAT,4,false);
    m_L.VAO.AddBuffer(m_L.VBO,m_L.VBL);

    m_P.S.Bind();
    m_P.S.SetUniformMat4f("u_PM",m_Proj);

    m_L.S.Bind();
    m_L.S.SetUniformMat4f("u_PM",m_Proj);

    m_T.S.Bind();
    m_T.S.SetUniformMat4f("u_PM",m_Proj);
    m_T.S.SetUniform1iv("texID",m_Slots,32);

    m_SPostProcessing.Bind();
    m_SPostProcessing.SetUniformMat4f("u_PM",m_Proj);
    m_SPostProcessing.SetUniform1iv("texID",m_Slots,32);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(5);
    glLineWidth(5);

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&m_MaxTextureSlots);
}

Renderer::~Renderer(){
    delete m_Framebuffer;
}

void Renderer::AddLayout(VertexBufferLayout &VBL,unsigned int type,unsigned int count,bool normalized){
    VBL.Push(type,count,normalized);
}

void Renderer::DrawTexture(float x,float y,float w,float h,float scale,float depth,float texID){
    auto [a,b,c,d]=VertexBuffer::CreateQuad(x,y,w,h,scale,depth,texID);
    m_BufferT[m_T.NumVertices]=a;
    m_BufferT[m_T.NumVertices+1]=b;
    m_BufferT[m_T.NumVertices+2]=c;
    m_BufferT[m_T.NumVertices+3]=d;
    m_T.NumVertices+=4;

    if(m_T.NumVertices==MAX_VERTICES){ 
        Render();
        m_T.NumVertices=0;
    }
}

void Renderer::DrawQuad(Vertex a,Vertex b,Vertex c,Vertex d){
    m_BufferT[m_T.NumVertices]=a;
    m_BufferT[m_T.NumVertices+1]=b;
    m_BufferT[m_T.NumVertices+2]=c;
    m_BufferT[m_T.NumVertices+3]=d;
    m_T.NumVertices+=4;

    if(m_T.NumVertices==MAX_VERTICES)
        Render();
}

void Renderer::DrawPoint(float x,float y,float r,float g,float b,float a){
    m_BufferP[m_P.NumVertices].Pos={x,y};
    m_BufferP[m_P.NumVertices].Color={r,g,b,a};
    ++m_P.NumVertices;

    if(m_P.NumVertices==MAX_VERTICES)
        Render();
}

void Renderer::DrawLine(float x1,float y1,float x2,float y2,float *color){
    m_BufferL[m_L.NumVertices].Pos={x1,y1};
    m_BufferL[m_L.NumVertices].Color={color[0],color[1],color[2],color[3]};
    m_BufferL[m_L.NumVertices+1].Pos={x2,y2};
    m_BufferL[m_L.NumVertices+1].Color=m_BufferL[m_L.NumVertices].Color;
    m_L.NumVertices+=2;

    if(m_L.NumVertices==MAX_VERTICES)
        Render();
}

void Renderer::StartScene(){
    if(PROJ_UPDATE){
        PROJ_UPDATE=false;
        m_Proj=glm::ortho(0.0f,(float)SCREEN_WIDTH,0.0f,(float)SCREEN_HEIGHT,-1.0f,1.0f);

        m_P.S.Bind();
        m_P.S.SetUniformMat4f("u_PM",m_Proj);

        m_L.S.Bind();
        m_L.S.SetUniformMat4f("u_PM",m_Proj);

        m_T.S.Bind();
        m_T.S.SetUniformMat4f("u_PM",m_Proj);

        m_SPostProcessing.Bind();
        m_SPostProcessing.SetUniformMat4f("u_PM",m_Proj);
    }
    if(FRAMEBUFFER_UPDATE){
        FRAMEBUFFER_UPDATE=false;
        delete m_Framebuffer;
        m_Framebuffer=new Framebuffer();
    }
    m_Framebuffer->Bind();
    Clear({0.0f,0.0f,0.0f});
}

void Renderer::DrawScene(){
    m_Framebuffer->Unbind();
    Clear({0.0f,0.0f,0.0f});
    DrawTexture(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,1,0,m_Framebuffer->GetColorbufferID());
    Render(true);
}

void Renderer::Render(bool finalScene){ //if this function gets called because there are MAX_VERTICES vertices, it's not guaranteed that it will respect depth input for subsequent vertices
    if(m_T.NumVertices==0 && m_P.NumVertices==0 && m_L.NumVertices==0)
        return;

    int lastChecked=-1;
    int slot=-1;
    int lastIndex=0;

    if(m_T.NumVertices>0){ //draw textures
        std::stable_sort(begin(m_BufferT),begin(m_BufferT)+m_T.NumVertices,cmp); //sorting textures by id to reduce the times you have to bind new textures
        m_T.VAO.Bind();
        m_T.VBO.Bind();
        IB.Bind();

        if(m_PostProcessingIndex!=std::numeric_limits<unsigned int>::max() && finalScene)
            PostProcessing();
        else
            m_T.S.Bind();

        for(unsigned int i=0;i<m_T.NumVertices;i++){
            if(m_BufferT[i].TexID!=lastChecked){ //new texture
                if(slot<m_MaxTextureSlots-1){ //slot available, change last id checked, increment slot and update the vertex
                    lastChecked=m_BufferT[i].TexID;
                    ++slot;
                    glActiveTexture(GL_TEXTURE0+slot);
                    glBindTexture(GL_TEXTURE_2D,lastChecked);
                    m_BufferT[i].TexID=(float)slot;
                }else{ //no slots available
                    m_T.VBO.SetData(0,(float *)&m_BufferT[lastIndex],i-lastIndex,sizeof(Vertex)); //send the data to the vertex buffer
                    glDrawElements(GL_TRIANGLES,(i-lastIndex)/4*6,GL_UNSIGNED_INT,nullptr); //draw
                    lastIndex=i; //update starting point for the next batch
                    lastChecked=m_BufferT[i].TexID;
                    slot=0;
                    m_BufferT[i].TexID=(float)slot;
                    DRAW_CALLS++;
                }
            }else{ //same texture, just update the vertex
                m_BufferT[i].TexID=(float)slot;
            }
        }
        if(m_T.NumVertices-lastIndex>0){ //if there are some vertices remaining, render them
            m_T.VBO.SetData(0,(float *)&m_BufferT[lastIndex],m_T.NumVertices-lastIndex,sizeof(Vertex));
            glDrawElements(GL_TRIANGLES,(m_T.NumVertices-lastIndex)/4*6,GL_UNSIGNED_INT,nullptr);
            DRAW_CALLS++;
        }
    }

    if(m_P.NumVertices>0){
        m_P.VAO.Bind();
        m_P.VBO.Bind();

        if(m_PostProcessingIndex!=std::numeric_limits<unsigned int>::max() && finalScene)
            PostProcessing();
        else
            m_P.S.Bind();

        m_P.VBO.SetData(0,(float *)&m_BufferP[0],m_P.NumVertices,sizeof(LinePointVertex));
        glDrawArrays(GL_POINTS,0,m_P.NumVertices);
    }

    if(m_L.NumVertices>0){
        m_L.VAO.Bind();    
        m_L.VBO.Bind();

        if(m_PostProcessingIndex!=std::numeric_limits<unsigned int>::max() && finalScene)
            PostProcessing();
        else
            m_L.S.Bind();

        m_L.VBO.SetData(0,(float *)&m_BufferL[0],m_L.NumVertices,sizeof(LinePointVertex));
        glDrawArrays(GL_LINES,0,m_L.NumVertices);
    }

    m_T.NumVertices=m_P.NumVertices=m_L.NumVertices=0;
}

void Renderer::Clear(Vec3 color) const{
    DRAW_CALLS=0;

    glClearColor(color.r,color.g,color.b,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::SetPostProcessing(const char *uniformName){
    if(uniformName==nullptr){
        m_PostProcessingIndex=std::numeric_limits<unsigned int>::max();
        return;
    }
    m_PostProcessingIndex=glGetSubroutineIndex(m_SPostProcessing.getID(),GL_FRAGMENT_SHADER,uniformName);;
}

void Renderer::PostProcessing(){
    m_SPostProcessing.Bind();
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER,1,&m_PostProcessingIndex);
}

void Renderer::ImGui_Init(){
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(WINDOW,true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    ImGui::GetIO().FontGlobalScale=1.5f; // Increase the font size (adjust the scale as needed)
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

    FPS=1.0/DELTA_TIME;
    ImGui::Text("FPS: %.1f",FPS);
    ImGui::Text("Frame Time: %.4f",DELTA_TIME*1000);

    if(ImGui::Checkbox("V-Sync",&V_SYNC))
        glfwSwapInterval(V_SYNC);
    
    if(ImGui::Checkbox("Fullscreen",&WindowInfo::ISFULLSCREEN))
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