#include <renderer.hpp>
#include <vector>
#include <algorithm>

#include <GLFW/glfw3.h>

Renderer::Renderer(): VB(MAX_QUADS),buffer(MAX_VERTICES),Num_Vertices(0),base_shader("resources/shaders/base/vertex.glsl","resources/shaders/base/fragment.glsl"){
    proj=glm::ortho(0.0f,(float)SCREEN_WIDTH,0.0f,(float)SCREEN_HEIGHT,-1.0f,1.0f);
    for(int i=0;i<32;i++)
        slots[i]=i;
    base_shader.Bind();
    base_shader.SetUniformMat4f("u_PM",proj); //set projection matrix
    base_shader.SetUniform1iv("texID",slots,32);

    Renderer::ImGui_Init(window);

    AddLayout(GL_FLOAT,2,false);
    AddLayout(GL_FLOAT,2,false);
    AddLayout(GL_FLOAT,1,false);
    VA.AddBuffer(VB,VBL);
}

void Renderer::AddLayout(unsigned int type,unsigned int count,bool normalized){
    VBL.Push(type,count,normalized);
}

void Renderer::Render(float x,float y,float w,float h,float scale,float texID){
    auto [a,b,c,d]=VertexBuffer::CreateQuad(x,y,w,h,scale,texID);
    buffer[Num_Vertices]=a;
    buffer[Num_Vertices+1]=b;
    buffer[Num_Vertices+2]=c;
    buffer[Num_Vertices+3]=d;
    Num_Vertices+=4;

    if(Num_Vertices==MAX_VERTICES){ 
        Draw();
        Num_Vertices=0;
    }
}

void Renderer::Render(Vertex a,Vertex b,Vertex c,Vertex d){
    buffer[Num_Vertices]=a;
    buffer[Num_Vertices+1]=b;
    buffer[Num_Vertices+2]=c;
    buffer[Num_Vertices+3]=d;
    Num_Vertices+=4;

    if(Num_Vertices==MAX_VERTICES){ 
        Draw();
        Num_Vertices=0;
    }
}

int Renderer::GetMaxTextureSlots(){
    int MaxTextureSlots;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&MaxTextureSlots);
    return MaxTextureSlots;
}

void Renderer::Draw(){
    if(Num_Vertices==0)
        return;

int lastChecked=-1;
int slot=-1;
int lastIndex=0;
int MaxTextureSlots=GetMaxTextureSlots();

    std::stable_sort(begin(buffer),begin(buffer)+Num_Vertices,cmp); //sorting textures by id to reduce the times you have to bind new textures
    VA.Bind();
    base_shader.Bind();
    IB.Bind();
    
    for(int i=0;i<Num_Vertices;i++){
        if(buffer[i].texID!=lastChecked){ //new texture
            if(slot<MaxTextureSlots-1){ //slot available, change last id checked, increment slot and update the vertex
                lastChecked=buffer[i].texID;
                ++slot;
                glActiveTexture(GL_TEXTURE0+slot);
                glBindTexture(GL_TEXTURE_2D,lastChecked);
                buffer[i].texID=(float)slot;
            }else{ //no slots available
                VB.SetData(0,(float *)&buffer[lastIndex],(i-lastIndex)/4); //send the data to the vertex buffer
                IB.Set((i-lastIndex)/4); //send the indices to the index buffer
                glDrawElements(GL_TRIANGLES,IB.GetNumElem(),GL_UNSIGNED_INT,nullptr); //draw
                lastIndex=i; //update starting point for the next batch
                lastChecked=buffer[i].texID;
                slot=0;
                buffer[i].texID=(float)slot;
                DRAW_CALLS++;
            }
        }else{ //same texture, just update the vertex
            buffer[i].texID=(float)slot;
        }
    }
    if(Num_Vertices-lastIndex>0){ //if there are some vertices remaining, render them
        VB.SetData(0,(float *)&buffer[lastIndex],(Num_Vertices-lastIndex)/4);
        IB.Set((Num_Vertices-lastIndex)/4);
        glDrawElements(GL_TRIANGLES,IB.GetNumElem(),GL_UNSIGNED_INT,nullptr);
        DRAW_CALLS++;
    }
    Num_Vertices=0;
}

void Renderer::Clear() const{
    DRAW_CALLS=0;

    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::ImGui_Init(GLFWwindow *window){
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    ImGui::GetIO().FontGlobalScale = 1.5f; // Increase the font size (adjust the scale as needed)
}

void Renderer::ImGui_Theme(){
ImVec4* colors = ImGui::GetStyle().Colors;
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
  style.WindowRounding                    = 7;
  style.ChildRounding                     = 4;
  style.FrameRounding                     = 3;
  style.PopupRounding                     = 4;
  style.ScrollbarRounding                 = 9;
  style.GrabRounding                      = 3;
  style.LogSliderDeadzone                 = 4;
  style.TabRounding                       = 4;
}

void Renderer::ImGui_Start_Frame(){
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void Renderer::ImGui_Performance(){
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(ImVec2(0,0));
    ImGui::Begin("FPS",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
    currentFrame=glfwGetTime();
    deltaTime=currentFrame-lastFrame;
    lastFrame=currentFrame;

    FPS=1.0/deltaTime;
    ImGui::Text("FPS: %.1f",FPS);

    if(ImGui::Checkbox("V-Sync",&v_sync)){
        glfwSwapInterval(v_sync);
    }
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