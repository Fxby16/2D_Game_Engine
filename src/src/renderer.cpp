#include <renderer.hpp>
#include <vector>

#include <GLFW/glfw3.h>

static int MaxTextureSlots=-1;

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

void Renderer::GetMaxTextureSlots(){
    if(MaxTextureSlots==-1)
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&MaxTextureSlots);
}

void Renderer::Draw(){
    if(Num_Vertices==0)
        return;
    DRAW_CALLS+=1;
    
    VA.Bind();
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

void Renderer::ImGui_Performance(){
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(ImVec2(0,0));
    ImGui::Begin("FPS",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
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