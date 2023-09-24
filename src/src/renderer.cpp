#include <renderer.hpp>
#include <vector>

#include <GLFW/glfw3.h>

#include <iostream>

static int MaxTextureSlots=-1;

void Renderer::AddLayout(unsigned int type,unsigned int count,bool normalized){
    VBL.Push(type,count,normalized);
}

void Renderer::Render(float x,float y,float w,float h,float texID){
    auto [a,b,c,d]=VertexBuffer::CreateQuad(x,y,w,h,texID);
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

void Renderer::GetMaxTextureSlots(){
    if(MaxTextureSlots==-1)
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&MaxTextureSlots);
}

void Renderer::Draw(){
    if(Num_Vertices==0)
        return;
    DRAW_CALLS+=1;

    VA.Bind();
    VA.AddBuffer(VB,VBL);
    VB.SetData(0,(float *)buffer.data(),Num_Vertices/4);
    base_shader.Bind();
    IB.Bind();
    IB.Set(Num_Vertices/4);
    glDrawElements(GL_TRIANGLES,IB.GetNumElem(),GL_UNSIGNED_INT,nullptr);
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

void Renderer::ImGui_Start_Frame(){
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void Renderer::ImGui_Body(){
    double currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    ImGui::Begin("Window");
    ImGui::SliderInt("NUM_QUADS",(int *)&NUM_QUADS,0,100000);
    double FPS=1.0/deltaTime;
    ImGui::Text("FPS: %.1f",FPS);
    ImGui::Text("DRAW CALLS: %d",DRAW_CALLS);
    if(ImGui::Checkbox("V-Sync",&v_sync)){
        glfwSwapInterval(v_sync); //v-sync
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