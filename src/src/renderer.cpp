#include <renderer.hpp>
#include <vector>
#include <algorithm>

#include <GLFW/glfw3.h>
#include <glfw.hpp>

RendererData::RendererData(const char *vertex_path,const char *fragment_path,unsigned int vertex_size): 
                          VBO(MAX_VERTICES,vertex_size,GL_DYNAMIC_DRAW),S(vertex_path,fragment_path),NumVertices(0){}

Renderer::Renderer(): 
    m_Textures("resources/shaders/textures/vertex.glsl","resources/shaders/textures/fragment.glsl",sizeof(Vertex)),
    m_Points("resources/shaders/points/vertex.glsl","resources/shaders/points/fragment.glsl",sizeof(LinePointVertex)),
    m_Lines("resources/shaders/lines/vertex.glsl","resources/shaders/lines/fragment.glsl",sizeof(LinePointVertex)),
    m_Triangles("resources/shaders/triangles/vertex.glsl","resources/shaders/triangles/fragment.glsl",sizeof(TriangleVertex)),
    m_Lights("resources/shaders/lights/vertex.glsl","resources/shaders/lights/fragment.glsl",4*sizeof(float)),
    m_BufferT(MAX_VERTICES),m_BufferP(MAX_VERTICES),m_BufferL(MAX_VERTICES),m_BufferTR(MAX_VERTICES),
    m_SPostProcessing("resources/shaders/textures/vertex.glsl","resources/shaders/post_processing/fragment.glsl"),
    m_PostProcessingIndex(std::numeric_limits<unsigned int>::max()){

    m_Framebuffer=new Framebuffer;
    m_LightingFramebuffer=new Framebuffer;
    //m_TempFramebuffer=new Framebuffer;
    IB.Set(MAX_VERTICES);

    m_Proj=glm::ortho(0.0f,(float)SCREEN_WIDTH,0.0f,(float)SCREEN_HEIGHT,-1.0f,1.0f);
    for(int i=0;i<32;i++)
        m_Slots[i]=i;

    AddLayout(m_Textures.VBL,GL_FLOAT,2,false);
    AddLayout(m_Textures.VBL,GL_FLOAT,2,false);
    AddLayout(m_Textures.VBL,GL_FLOAT,1,false);
    AddLayout(m_Textures.VBL,GL_FLOAT,1,false);
    m_Textures.VAO.AddBuffer(m_Textures.VBO,m_Textures.VBL);

    AddLayout(m_Points.VBL,GL_FLOAT,2,false);
    AddLayout(m_Points.VBL,GL_FLOAT,4,false);
    m_Points.VAO.AddBuffer(m_Points.VBO,m_Points.VBL);
    
    AddLayout(m_Lines.VBL,GL_FLOAT,2,false);
    AddLayout(m_Lines.VBL,GL_FLOAT,4,false);
    m_Lines.VAO.AddBuffer(m_Lines.VBO,m_Lines.VBL);

    AddLayout(m_Triangles.VBL,GL_FLOAT,2,false);
    AddLayout(m_Triangles.VBL,GL_FLOAT,4,false);
    m_Triangles.VAO.AddBuffer(m_Triangles.VBO,m_Triangles.VBL);

    AddLayout(m_Lights.VBL,GL_FLOAT,2,false);
    AddLayout(m_Lights.VBL,GL_FLOAT,2,false);
    m_Lights.VAO.AddBuffer(m_Lights.VBO,m_Lights.VBL);

    m_Points.S.Bind();
    m_Points.S.SetUniformMat4f("u_PM",m_Proj);
    m_Points.S.SetUniform1f("blurAmount",0.0f);

    m_Lines.S.Bind();
    m_Lines.S.SetUniformMat4f("u_PM",m_Proj);

    m_Textures.S.Bind();
    m_Textures.S.SetUniformMat4f("u_PM",m_Proj);
    m_Textures.S.SetUniform1iv("texID",m_Slots,32);

    m_Triangles.S.Bind();
    m_Triangles.S.SetUniformMat4f("u_PM",m_Proj);

    m_Lights.S.Bind();
    m_Lights.S.SetUniformMat4f("u_PM",m_Proj);
    m_Lights.S.SetUniform1i("framebuffer",0);
    m_Lights.S.SetUniform1i("light",1);

    m_SPostProcessing.Bind();
    m_SPostProcessing.SetUniformMat4f("u_PM",m_Proj);
    m_SPostProcessing.SetUniform1iv("texID",m_Slots,32);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_PROGRAM_POINT_SIZE);
    ChangePointSize(5);
    ChangeLineWidth(5);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&m_MaxTextureSlots);

    segments.push_back(std::make_pair(Vec2(0,0),Vec2(0,SCREEN_HEIGHT)));
    segments.push_back(std::make_pair(Vec2(0,SCREEN_HEIGHT),Vec2(SCREEN_WIDTH,SCREEN_HEIGHT)));
    segments.push_back(std::make_pair(Vec2(SCREEN_WIDTH,SCREEN_HEIGHT),Vec2(SCREEN_WIDTH,0)));
    segments.push_back(std::make_pair(Vec2(SCREEN_WIDTH,0),Vec2(0,0)));

    Renderer::ImGui_Init();
}

Renderer::~Renderer(){
    delete m_Framebuffer;
}

void Renderer::BindLightingFB(){
    m_LightingFramebuffer->Bind();
}

void Renderer::ApplyLight(){
    float vertices[]={ 0.0f,0.0f,0.0f,0.0f, 
                       0.0f,static_cast<float>(SCREEN_HEIGHT),0.0f,1.0f,
                       static_cast<float>(SCREEN_WIDTH),static_cast<float>(SCREEN_HEIGHT),1.0f,1.0f,
                       static_cast<float>(SCREEN_WIDTH),0.0f,1.0f,0.0f};

    m_Lights.VBO.Bind();
    m_Lights.VBO.SetData(0,vertices,4,4*sizeof(float));

    //glBindFramebuffer(GL_READ_FRAMEBUFFER,m_Framebuffer->GetFramebufferID());
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER,m_TempFramebuffer->GetFramebufferID());
    //glBlitFramebuffer(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,GL_COLOR_BUFFER_BIT,GL_NEAREST);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_Framebuffer->GetColorbufferID());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,m_LightingFramebuffer->GetColorbufferID());
    
    m_Framebuffer->Bind();
    IB.Bind();
    m_Lights.VAO.Bind();
    m_Lights.S.Bind();

    int sub_id=Shader::GetSubroutineIndex("Merge",m_Lights.S.getID());
    Shader::SetSubroutineUniform(sub_id);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);
}

void Renderer::AddLayout(VertexBufferLayout &VBL,unsigned int type,unsigned int count,bool normalized){
    VBL.Push(type,count,normalized);
}

void Renderer::DrawTexture(float x,float y,float w,float h,float scale,float depth,float texID){
    auto [a,b,c,d]=VertexBuffer::CreateQuad(x,y,w,h,scale,depth,texID);
    m_BufferT[m_Textures.NumVertices]=a;
    m_BufferT[m_Textures.NumVertices+1]=b;
    m_BufferT[m_Textures.NumVertices+2]=c;
    m_BufferT[m_Textures.NumVertices+3]=d;
    m_Textures.NumVertices+=4;

    if(m_Textures.NumVertices==MAX_VERTICES)
        Render();
}

void Renderer::DrawTriangle(float x1,float y1,float x2,float y2,float x3,float y3,float r,float g,float b,float a){
    m_BufferTR[m_Triangles.NumVertices]=TriangleVertex(Vec2(x1,y1),Vec4(r,g,b,a));
    m_BufferTR[m_Triangles.NumVertices+1]=TriangleVertex(Vec2(x2,y2),Vec4(r,g,b,a));
    m_BufferTR[m_Triangles.NumVertices+2]=TriangleVertex(Vec2(x3,y3),Vec4(r,g,b,a));
    m_Triangles.NumVertices+=3;

    if(m_Triangles.NumVertices==MAX_VERTICES)
        Render();
}

void Renderer::DrawQuad(Vertex a,Vertex b,Vertex c,Vertex d){
    m_BufferT[m_Textures.NumVertices]=a;
    m_BufferT[m_Textures.NumVertices+1]=b;
    m_BufferT[m_Textures.NumVertices+2]=c;
    m_BufferT[m_Textures.NumVertices+3]=d;
    m_Textures.NumVertices+=4;

    if(m_Textures.NumVertices==MAX_VERTICES)
        Render();
}

void Renderer::DrawSolidQuad(float x,float y,float w,float h,Vec4 color){
    DrawTriangle(x,y,x+w,y,x,y+h,color.r,color.g,color.b,color.a);
    DrawTriangle(x,y+h,x+w,y+h,x+w,y,color.r,color.g,color.b,color.a);
}

void Renderer::DrawPoint(float x,float y,float r,float g,float b,float a){
    m_BufferP[m_Points.NumVertices].Pos={x,y};
    m_BufferP[m_Points.NumVertices].Color={r,g,b,a};
    ++m_Points.NumVertices;

    if(m_Points.NumVertices==MAX_VERTICES)
        Render();
}

void Renderer::DrawLine(float x1,float y1,float x2,float y2,float r,float g,float b,float a){
    m_BufferL[m_Lines.NumVertices].Pos={x1,y1};
    m_BufferL[m_Lines.NumVertices].Color={r,g,b,a};
    m_BufferL[m_Lines.NumVertices+1].Pos={x2,y2};
    m_BufferL[m_Lines.NumVertices+1].Color=m_BufferL[m_Lines.NumVertices].Color;
    m_Lines.NumVertices+=2;

    if(m_Lines.NumVertices==MAX_VERTICES)
        Render();
}

void Renderer::ChangeLineWidth(float new_size){
    glLineWidth(new_size);
}

void Renderer::ChangePointSize(float new_size){
    glPointSize(new_size);
}

void Renderer::StartScene(){
    if(PROJ_UPDATE){
        PROJ_UPDATE=false;
        m_Proj=glm::ortho(0.0f,(float)SCREEN_WIDTH,0.0f,(float)SCREEN_HEIGHT,-1.0f,1.0f);

        m_Points.S.Bind();
        m_Points.S.SetUniformMat4f("u_PM",m_Proj);

        m_Lines.S.Bind();
        m_Lines.S.SetUniformMat4f("u_PM",m_Proj);

        m_Textures.S.Bind();
        m_Textures.S.SetUniformMat4f("u_PM",m_Proj);

        m_Triangles.S.Bind();
        m_Triangles.S.SetUniformMat4f("u_PM",m_Proj);

        m_Lights.S.Bind();
        m_Lights.S.SetUniformMat4f("u_PM",m_Proj);

        m_SPostProcessing.Bind();
        m_SPostProcessing.SetUniformMat4f("u_PM",m_Proj);
    }
    if(FRAMEBUFFER_UPDATE){
        FRAMEBUFFER_UPDATE=false;
        delete m_Framebuffer;
        m_Framebuffer=new Framebuffer;
        delete m_LightingFramebuffer;
        m_LightingFramebuffer=new Framebuffer;
        //delete m_TempFramebuffer;
        //m_TempFramebuffer=new Framebuffer;
    }
    BindLightingFB();
    Clear({0.3f,0.3f,0.3f});
    m_Framebuffer->Bind();
    Clear({0.0f,0.0f,0.0f});
}

void Renderer::DrawScene(){
    m_Framebuffer->Unbind();
    Clear({0.0f,0.0f,0.0f});
    DrawTexture(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,1,0,m_Framebuffer->GetColorbufferID());
    Render(true);
}

void Renderer::Render(bool post_processing){ //if this function gets called because there are MAX_VERTICES vertices, it's not guaranteed that it will respect depth input for subsequent vertices
    RenderTextures(post_processing);
    RenderPoints();
    RenderLines();
    RenderTriangles();
}

void Renderer::RenderTextures(bool post_processing){
    if(m_Textures.NumVertices<=0)
        return;

    int lastChecked=-1;
    int slot=-1;
    int lastIndex=0;

    std::stable_sort(begin(m_BufferT),begin(m_BufferT)+m_Textures.NumVertices,cmp); //sorting textures by id to reduce the times you have to bind new textures
    m_Textures.VAO.Bind();
    m_Textures.VBO.Bind();
    IB.Bind();

    if(m_PostProcessingIndex!=std::numeric_limits<unsigned int>::max() && post_processing)
        PostProcessing();
    else
        m_Textures.S.Bind();

    for(unsigned int i=0;i<m_Textures.NumVertices;i++){
        if(m_BufferT[i].TexID!=lastChecked){ //new texture
            if(slot<m_MaxTextureSlots-1){ //slot available, change last id checked, increment slot and update the vertex
                lastChecked=m_BufferT[i].TexID;
                ++slot;
                glActiveTexture(GL_TEXTURE0+slot);
                glBindTexture(GL_TEXTURE_2D,lastChecked);
                m_BufferT[i].TexID=(float)slot;
            }else{ //no slots available
                m_Textures.VBO.SetData(0,(float *)&m_BufferT[lastIndex],i-lastIndex,sizeof(Vertex)); //send the data to the vertex buffer
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
    if(m_Textures.NumVertices-lastIndex>0){ //if there are some vertices remaining, render them
        m_Textures.VBO.SetData(0,(float *)&m_BufferT[lastIndex],m_Textures.NumVertices-lastIndex,sizeof(Vertex));
        glDrawElements(GL_TRIANGLES,(m_Textures.NumVertices-lastIndex)/4*6,GL_UNSIGNED_INT,nullptr);
        DRAW_CALLS++;
    }

    m_Textures.NumVertices=0;
}

void Renderer::RenderTriangles(){
    if(m_Triangles.NumVertices<=0)
        return;

    m_Triangles.VAO.Bind();
    m_Triangles.VBO.Bind();
    m_Triangles.S.Bind();

    m_Triangles.VBO.SetData(0,(float *)&m_BufferTR[0],m_Triangles.NumVertices,sizeof(TriangleVertex));
    glDrawArrays(GL_TRIANGLES,0,m_Triangles.NumVertices);

    m_Triangles.NumVertices=0;
}

void Renderer::RenderPoints(){
    if(m_Points.NumVertices<=0)
        return;

    m_Points.VAO.Bind();
    m_Points.VBO.Bind();
    m_Points.S.Bind();

    m_Points.VBO.SetData(0,(float *)&m_BufferP[0],m_Points.NumVertices,sizeof(LinePointVertex));
    glDrawArrays(GL_POINTS,0,m_Points.NumVertices);

    m_Points.NumVertices=0;
}

void Renderer::RenderLines(){
    if(m_Lines.NumVertices<=0)
        return;

    m_Lines.VAO.Bind();    
    m_Lines.VBO.Bind();
    m_Lines.S.Bind();

    m_Lines.VBO.SetData(0,(float *)&m_BufferL[0],m_Lines.NumVertices,sizeof(LinePointVertex));
    glDrawArrays(GL_LINES,0,m_Lines.NumVertices);

    m_Lines.NumVertices=0;
}

void Renderer::Clear(Vec3 color) const{
    DRAW_CALLS=0;

    glClearColor(color.r,color.g,color.b,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::SetPostProcessing(const char *uniform_name){
    m_PostProcessingIndex=Shader::GetSubroutineIndex(uniform_name,m_SPostProcessing.getID());
}

void Renderer::PostProcessing(){
    m_SPostProcessing.Bind();
    Shader::SetSubroutineUniform(m_PostProcessingIndex);
}

void Renderer::AddSegment(Vec2 start_point,Vec2 end_point){
    segments.push_back(std::make_pair(start_point,end_point));
}

void Renderer::UpdateScreenSegments(){
    segments[0]=std::make_pair(Vec2(0,0),Vec2(0,SCREEN_HEIGHT));
    segments[1]=std::make_pair(Vec2(0,SCREEN_HEIGHT),Vec2(SCREEN_WIDTH,SCREEN_HEIGHT));
    segments[2]=std::make_pair(Vec2(SCREEN_WIDTH,SCREEN_HEIGHT),Vec2(SCREEN_WIDTH,0));
    segments[3]=std::make_pair(Vec2(SCREEN_WIDTH,0),Vec2(0,0)); 
}

void Renderer::ClearSegments(){
    segments.resize(4);
    UpdateScreenSegments();
}

void Renderer::DrawLight(float light_x,float light_y,Vec4 color,enum LightType light_type,float radius,float blurAmount){
    if(light_type==ALL_LIGHT){
        std::vector<Vec2>points;
        auto find=[&points](Vec2 point)->bool{
            for(size_t i=0;i<points.size();i++){
                if(points[i].x==point.x && points[i].y==point.y)
                    return false;
            }
            return true;
        };
        for(auto [a,b]:segments){
            if(find(a))
                points.push_back(a);
            if(find(b))
                points.push_back(b);

            DrawLine(a.x,a.y,b.x,b.y,0.5f,1.0f,1.0f,1.0f);
        }
        std::vector<float>angles;

        for(auto p:points){
            float angle=std::atan2(p.y-light_y,p.x-light_x);
            angles.push_back(angle-0.0001f);
            angles.push_back(angle);
            angles.push_back(angle+0.0001f);
        }

        auto GetIntersection=[](std::pair<Vec2,Vec2>ray,std::pair<Vec2,Vec2>seg)->std::pair<Vec2,float>{
            float r_px=ray.first.x;
            float r_py=ray.first.y;
            float r_dx=ray.second.x-ray.first.x;
            float r_dy=ray.second.y-ray.first.y;
        
            float s_px=seg.first.x;
            float s_py=seg.first.y;
            float s_dx=seg.second.x-seg.first.x;
            float s_dy=seg.second.y-seg.first.y;
        
            float r_mag=std::sqrt(r_dx*r_dx+r_dy*r_dy);
            float s_mag=std::sqrt(s_dx*s_dx+s_dy*s_dy);
            if(r_dx/r_mag==s_dx/s_mag && r_dy/r_mag==s_dy/s_mag){
                return std::make_pair(Vec2(-1.0f,-1.0f),-1.0f); 
            }

            float T2=(r_dx*(s_py-r_py)+r_dy*(r_px-s_px))/(s_dx*r_dy-s_dy*r_dx);
            float T1=(s_px+s_dx*T2-r_px)/r_dx;
        
            if(T1<0) return std::make_pair(Vec2(-1.0f,-1.0f),-1.0f);
            if(T2<0 || T2>1) return std::make_pair(Vec2(-1.0f,-1.0f),-1.0f);
        
            return std::make_pair(Vec2(r_px+r_dx*T1,r_py+r_dy*T1),T1);
        };

        std::vector<std::pair<Vec2,float>>intersects;
        for(auto angle:angles){
            float dx=std::cos(angle);
            float dy=std::sin(angle);

            std::pair<Vec2,Vec2>ray=std::make_pair(Vec2(light_x,light_y),Vec2(light_x+dx,light_y+dy));
            std::pair<Vec2,double> closest_intersect=std::make_pair(Vec2(-1.0f,-1.0f),-1.0f);
            for(auto seg:segments){
                std::pair<Vec2,double> intersect=GetIntersection(ray,seg);
                if(intersect.first.x==-1.0f && intersect.first.y==-1.0f)
                    continue;
                if((closest_intersect.first.x==-1.0f && closest_intersect.first.y==-1.0f) || (intersect.second<closest_intersect.second && intersect.second!=-1.0f))
                    closest_intersect=intersect;
            }
            if(closest_intersect.first.x!=-1.0f && closest_intersect.first.y!=-1.0f)
                intersects.push_back(std::make_pair(closest_intersect.first,angle));
        }
        
        //for(auto p:intersects){
        //    DrawLine(p.first.x,p.first.y,light_x,light_y,0,1,0,1);
        //    DrawPoint(p.first.x,p.first.y,0,0,1,1);
        //}

        std::sort(begin(intersects),end(intersects),[](const auto& a,const auto& b){
            return a.second<b.second;
        });

        Render();

        m_LightingFramebuffer->Bind();
        glBlendFunc(GL_ONE,GL_ONE);

        for(size_t i=0;i<intersects.size()-1;i++){
            auto a=intersects[i].first;
            auto b=intersects[i+1].first;
            DrawTriangle(light_x,light_y,a.x,a.y,b.x,b.y,color.r,color.g,color.b,color.a);
        }
        DrawTriangle(light_x,light_y,intersects[0].first.x,intersects[0].first.y,intersects[intersects.size()-1].first.x,intersects[intersects.size()-1].first.y,color.r,color.g,color.b,color.a);
        RenderTriangles();
    }else{
        Render();
        float previousSize;
        glGetFloatv(GL_POINT_SIZE,&previousSize);
        ChangePointSize(radius*2);
        m_Points.S.Bind();
        m_Points.S.SetUniform1f("blurAmount",blurAmount);
        m_LightingFramebuffer->Bind();
        glBlendFunc(GL_ONE,GL_ONE);
        DrawPoint(light_x,light_y,color.r,color.g,color.b,color.a);
        RenderPoints();
        m_Points.S.SetUniform1f("blurAmount",0.0f);
        ChangePointSize(previousSize);
    }
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
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