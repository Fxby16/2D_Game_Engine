#include <renderer.hpp>
#include <vector>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <window.hpp>
#include <memory.hpp>
#include <Instrumentor.h>

RendererData::RendererData(const char *vertex_path,const char *fragment_path,unsigned int vertex_size): 
                          VBO(MAX_VERTICES,vertex_size,GL_DYNAMIC_DRAW),S(vertex_path,fragment_path),NumVertices(0){}

Renderer::Renderer(): 
    m_Textures("resources/shaders/textures/vertex.glsl","resources/shaders/textures/fragment.glsl",sizeof(Vertex)),
    m_Points("resources/shaders/points/vertex.glsl","resources/shaders/points/fragment.glsl",sizeof(LinePointVertex)),
    m_Lines("resources/shaders/lines/vertex.glsl","resources/shaders/lines/fragment.glsl",sizeof(LinePointVertex)),
    m_Triangles("resources/shaders/triangles/vertex.glsl","resources/shaders/triangles/fragment.glsl",sizeof(TriangleVertex)),
    m_Lights("resources/shaders/lights/vertex.glsl","resources/shaders/lights/fragment.glsl",4*sizeof(float)),
    m_SPostProcessing("resources/shaders/textures/vertex.glsl","resources/shaders/post_processing/fragment.glsl"),
    m_PostProcessingIndex(std::numeric_limits<unsigned int>::max()),m_AmbientLight(0.0f,0.0f,0.0f),m_ClearColor(0.0f,0.0f,0.0f){

    float vertices[]={ 0.0f,0.0f,0.0f,0.0f, 
                    0.0f,Window::MAX_HEIGHT,0.0f,1.0f,
                    Window::MAX_WIDTH,Window::MAX_HEIGHT,1.0f,1.0f,
                    Window::MAX_WIDTH,0.0f,1.0f,0.0f};

    m_BufferT=(Vertex *)AllocateMemory(MAX_VERTICES*sizeof(Vertex));
    m_BufferP=(LinePointVertex *)AllocateMemory(MAX_VERTICES*sizeof(LinePointVertex));
    m_BufferL=(LinePointVertex *)AllocateMemory(MAX_VERTICES*sizeof(LinePointVertex));
    m_BufferTR=(TriangleVertex *)AllocateMemory(MAX_VERTICES*sizeof(TriangleVertex));

    m_Framebuffer=new Framebuffer;
    m_LightingFramebuffer=new Framebuffer;
    m_TempFramebuffer=new Framebuffer;
    IB.Set(MAX_VERTICES);

    for(int i=0;i<32;i++)
        m_Slots[i]=i;

    AddLayout(m_Textures.VBL,GL_FLOAT,2,false);
    AddLayout(m_Textures.VBL,GL_FLOAT,2,false);
    AddLayout(m_Textures.VBL,GL_FLOAT,1,false);
    AddLayout(m_Textures.VBL,GL_FLOAT,1,false);
    m_Textures.VAO.AddBuffer(m_Textures.VBO,m_Textures.VBL);

    AddLayout(m_Points.VBL,GL_FLOAT,2,false);
    AddLayout(m_Points.VBL,GL_FLOAT,4,false);
    AddLayout(m_Points.VBL,GL_FLOAT,1,false);
    m_Points.VAO.AddBuffer(m_Points.VBO,m_Points.VBL);
    
    AddLayout(m_Lines.VBL,GL_FLOAT,2,false);
    AddLayout(m_Lines.VBL,GL_FLOAT,4,false);
    AddLayout(m_Lines.VBL,GL_FLOAT,1,false);
    m_Lines.VAO.AddBuffer(m_Lines.VBO,m_Lines.VBL);

    AddLayout(m_Triangles.VBL,GL_FLOAT,2,false);
    AddLayout(m_Triangles.VBL,GL_FLOAT,4,false);
    AddLayout(m_Triangles.VBL,GL_FLOAT,1,false);
    m_Triangles.VAO.AddBuffer(m_Triangles.VBO,m_Triangles.VBL);

    AddLayout(m_Lights.VBL,GL_FLOAT,2,false);
    AddLayout(m_Lights.VBL,GL_FLOAT,2,false);
    m_Lights.VAO.AddBuffer(m_Lights.VBO,m_Lights.VBL);
    m_Lights.VBO.SetData(0,vertices,4,4*sizeof(float));

    m_Points.S.Bind();
    m_Points.S.SetUniform1f("blurAmount",0.0f);

    m_Textures.S.Bind();
    m_Textures.S.SetUniform1iv("texID",m_Slots,32);

    m_Lights.S.Bind();
    m_Lights.S.SetUniform1i("framebuffer",0);
    m_Lights.S.SetUniform1i("light",1);

    m_SPostProcessing.Bind();
    m_SPostProcessing.SetUniform1iv("texID",m_Slots,32);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_PROGRAM_POINT_SIZE);
    SetPointSize(1.0f);
    SetLineWidth(0.1f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_PROGRAM_POINT_SIZE);

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&m_MaxTextureSlots);
    #ifdef DEBUG
        printf("Max texture slots: %d\n",m_MaxTextureSlots);
    #endif

    segments.push_back(std::make_pair(Vec2(0,0),Vec2(0,Window::MAX_HEIGHT)));
    segments.push_back(std::make_pair(Vec2(0,Window::MAX_HEIGHT),Vec2(Window::MAX_WIDTH,Window::MAX_HEIGHT)));
    segments.push_back(std::make_pair(Vec2(Window::MAX_WIDTH,Window::MAX_HEIGHT),Vec2(Window::MAX_WIDTH,0)));
    segments.push_back(std::make_pair(Vec2(Window::MAX_WIDTH,0),Vec2(0,0)));

    m_AmbientLight=Vec3(0.0f,0.0f,0.0f);
}

Renderer::~Renderer(){
    FreeMemory(m_BufferT);
    FreeMemory(m_BufferP);
    FreeMemory(m_BufferL);
    FreeMemory(m_BufferTR);

    delete m_Framebuffer;
    delete m_LightingFramebuffer;
}

void Renderer::AddLayout(VertexBufferLayout &VBL,unsigned int type,unsigned int count,bool normalized){
    VBL.Push(type,count,normalized);
}

void Renderer::DrawTexture(Vec2 pos,Vec2 size,float layer,float texID){
    PROFILE_FUNCTION();

    auto [a,b,c,d]=VertexBuffer::CreateQuad(pos.x,pos.y,size.w,size.h,layer,texID);
    m_BufferT[m_Textures.NumVertices]=a;
    m_BufferT[m_Textures.NumVertices+1]=b;
    m_BufferT[m_Textures.NumVertices+2]=c;
    m_BufferT[m_Textures.NumVertices+3]=d;
    m_Textures.NumVertices+=4;

    if(m_Textures.NumVertices==MAX_VERTICES)
        Render();
}

void Renderer::DrawTexture(Vec2 pos,Vec2 size,Vec2 texture_pos,Vec2 texture_size,Vec2 texture_total_size,float layer,float texID){
    PROFILE_FUNCTION();
    
    auto [a,b,c,d]=VertexBuffer::CreateQuad(pos.x,pos.y,size.w,size.h,texture_pos.x,texture_pos.y,texture_size.w,texture_size.h,texture_total_size.w,texture_total_size.h,layer,texID);
    m_BufferT[m_Textures.NumVertices]=a;
    m_BufferT[m_Textures.NumVertices+1]=b;
    m_BufferT[m_Textures.NumVertices+2]=c;
    m_BufferT[m_Textures.NumVertices+3]=d;
    m_Textures.NumVertices+=4;

    if(m_Textures.NumVertices==MAX_VERTICES)
        Render();
}

void Renderer::DrawTexture(Vec2 pos,Vec2 size,bool reverse_x,bool reverse_y,float layer,Texture &texture){
    PROFILE_FUNCTION();
    
    if(reverse_x && reverse_y)
        DrawTexture(pos,size,{texture.GetWidth(),texture.GetHeight()},{-texture.GetWidth(),-texture.GetHeight()},{texture.GetWidth(),texture.GetHeight()},layer,texture.GetTexID());
    else if(reverse_x)
        DrawTexture(pos,size,{texture.GetWidth(),0},{-texture.GetWidth(),texture.GetHeight()},{texture.GetWidth(),texture.GetHeight()},layer,texture.GetTexID());
    else if(reverse_y)
        DrawTexture(pos,size,{0,texture.GetHeight()},{texture.GetWidth(),-texture.GetHeight()},{texture.GetWidth(),texture.GetHeight()},layer,texture.GetTexID());
    else
        DrawTexture(pos,size,layer,texture.GetTexID());
}

void Renderer::DrawSpriteSheet(Vec2 pos,Vec2 size,float row,float col,float layer,SpriteSheet &s){
    PROFILE_FUNCTION();
    
    std::array<Vertex,4>quad=s.CreateQuadSpriteSheet(pos.x,pos.y,size.w,size.h,row,col,layer,s.GetTexID());
    m_BufferT[m_Textures.NumVertices]=quad[0];
    m_BufferT[m_Textures.NumVertices+1]=quad[1];
    m_BufferT[m_Textures.NumVertices+2]=quad[2];
    m_BufferT[m_Textures.NumVertices+3]=quad[3];
    m_Textures.NumVertices+=4;

    if(m_Textures.NumVertices==MAX_VERTICES)
        Render();
}

void Renderer::DrawAnimatedTexture(Vec2 pos,Vec2 size,float layer,AnimatedTexture &at){
    PROFILE_FUNCTION();
    
    if(at.m_PlayAnimation){
        if(glfwGetTime()-at.m_LastAnimationTime>=at.m_AnimationDelay){
            at.m_LastAnimationTime=glfwGetTime();
            at.m_AnimationIndex++;
            if(at.m_AnimationIndex>=at.m_Width/at.m_TileWidth){
                if(!at.m_LoopAnimation){
                    at.m_PlayAnimation=false;
                }
                at.m_AnimationIndex=0;
            }
        }
    }
    DrawSpriteSheet(pos,size,ceil((float)at.m_Height/(float)at.m_TileHeight)-1,at.m_AnimationIndex,layer,at);
}


void Renderer::DrawTriangle(Vec2 pos1,Vec2 pos2,Vec2 pos3,Vec4 color,float layer){
    PROFILE_FUNCTION();
    
    m_BufferTR[m_Triangles.NumVertices]=TriangleVertex(pos1,color,layer);
    m_BufferTR[m_Triangles.NumVertices+1]=TriangleVertex(pos2,color,layer);
    m_BufferTR[m_Triangles.NumVertices+2]=TriangleVertex(pos3,color,layer);
    m_Triangles.NumVertices+=3;

    if(m_Triangles.NumVertices==MAX_VERTICES)
        Render();
}

void Renderer::DrawQuad(Vertex a,Vertex b,Vertex c,Vertex d){
    PROFILE_FUNCTION();
    
    m_BufferT[m_Textures.NumVertices]=a;
    m_BufferT[m_Textures.NumVertices+1]=b;
    m_BufferT[m_Textures.NumVertices+2]=c;
    m_BufferT[m_Textures.NumVertices+3]=d;
    m_Textures.NumVertices+=4;

    if(m_Textures.NumVertices==MAX_VERTICES)
        Render();
}

void Renderer::DrawSolidQuad(Vec2 pos,Vec2 size,Vec4 color,float layer){
    PROFILE_FUNCTION();
    
    DrawTriangle(pos,{pos.x+size.w,pos.y},{pos.x,pos.y+size.h},color,layer);
    DrawTriangle({pos.x,pos.y+size.h},{pos.x+size.w,pos.y+size.h},{pos.x+size.w,pos.y},color,layer);
}

void Renderer::DrawPoint(Vec2 pos,Vec4 color,float layer){
    PROFILE_FUNCTION();
    
    m_BufferP[m_Points.NumVertices]=LinePointVertex(pos,color,layer);
    ++m_Points.NumVertices;

    if(m_Points.NumVertices==MAX_VERTICES)
        Render();
}

void Renderer::DrawLine(Vec2 pos1,Vec2 pos2,Vec4 color,float layer){
    PROFILE_FUNCTION();
    
    m_BufferL[m_Lines.NumVertices]=LinePointVertex(pos1,color,layer);
    m_BufferL[m_Lines.NumVertices+1]=LinePointVertex(pos2,color,layer);
    m_Lines.NumVertices+=2;

    if(m_Lines.NumVertices==MAX_VERTICES)
        Render();
}

void Renderer::SetLineWidth(float new_size){
    m_LineWidth=new_size;
    glLineWidth((new_size*m_Zoom)/Window::MAX_WIDTH*Window::Width);
}

void Renderer::UpdateLineWidth(){
    glLineWidth((m_LineWidth*m_Zoom)/Window::MAX_WIDTH*Window::Width);
}

/**
 * \param new_size the new diameter
*/
void Renderer::SetPointSize(float new_size){
    //glPointSize(new_size/Window::MAX_WIDTH*Window::Width);
    m_PointSize=new_size;
    m_Points.S.Bind();
    m_Points.S.SetUniform1f("pointSize",new_size/Window::MAX_WIDTH*Window::Width);
}

float Renderer::GetPointSize(){
    return m_PointSize; 
}

float Renderer::GetLineWidth(){
    return m_LineWidth;
}

void Renderer::SetAmbientLight(Vec3 color){
    m_AmbientLight=color;
}

void Renderer::SetClearColor(Vec3 color){
    m_ClearColor=color;
}

void Renderer::StartScene(){
    PROFILE_FUNCTION();
    
    m_TextureIndex=m_PointIndex=m_LineIndex=m_TriangleIndex=0;
    if(Window::FramebufferUpdate){
        Window::FramebufferUpdate=false;
        delete m_Framebuffer;
        m_Framebuffer=new Framebuffer;
        delete m_LightingFramebuffer;
        m_LightingFramebuffer=new Framebuffer;
        delete m_TempFramebuffer;
        m_TempFramebuffer=new Framebuffer;

        float vertices[]={ 0.0f,0.0f,0.0f,0.0f, 
                       0.0f,Window::MAX_HEIGHT,0.0f,1.0f,
                       Window::MAX_WIDTH,Window::MAX_HEIGHT,1.0f,1.0f,
                       Window::MAX_WIDTH,0.0f,1.0f,0.0f};

        m_Lights.VBO.Bind();
        m_Lights.VBO.SetData(0,vertices,4,4*sizeof(float));
    }
    m_TempFramebuffer->Bind();
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    m_LightingFramebuffer->Bind();
    Clear(true);
    m_Framebuffer->Bind();
    Clear();
}

void Renderer::DrawScene(){
    PROFILE_FUNCTION();
    
    m_Framebuffer->Unbind();
    Clear();
    DrawTexture({0,0},{Window::MAX_WIDTH,Window::MAX_HEIGHT},0,m_Framebuffer->GetColorbufferID());
    Render(true);
}

void Renderer::Render(bool post_processing){ //if this function gets called because there are MAX_VERTICES vertices, it's not guaranteed that it will respect layer input for subsequent vertices
    PROFILE_FUNCTION();
    
    if(m_Textures.NumVertices>0)
        std::stable_sort(m_BufferT,m_BufferT+m_Textures.NumVertices,cmp1);
    if(m_Points.NumVertices>0)
        std::stable_sort(m_BufferP,m_BufferP+m_Points.NumVertices,cmp2);
    if(m_Lines.NumVertices>0)
        std::stable_sort(m_BufferL,m_BufferL+m_Lines.NumVertices,cmp2);
    if(m_Triangles.NumVertices>0)
        std::stable_sort(m_BufferTR,m_BufferTR+m_Triangles.NumVertices,cmp3);

    float min_texture_layer,min_point_layer,min_line_layer,min_triangle_layer;
    
    while(true){
        if(m_Textures.NumVertices==0 && m_Points.NumVertices==0 && m_Lines.NumVertices==0 && m_Triangles.NumVertices==0)
            break;

        min_texture_layer=GetTexturesMinLayer();
        min_point_layer=GetPointsMinLayer();
        min_line_layer=GetLinesMinLayer();
        min_triangle_layer=GetTrianglesMinLayer();
        
        if(min_texture_layer!=std::numeric_limits<float>::max())    
            RenderTextures(post_processing,std::min({min_point_layer,min_line_layer,min_triangle_layer}));
        if(min_point_layer!=std::numeric_limits<float>::max())
            RenderPoints(std::min({min_texture_layer,min_line_layer,min_triangle_layer}));
        if(min_line_layer!=std::numeric_limits<float>::max())
            RenderLines(std::min({min_texture_layer,min_point_layer,min_triangle_layer}));
        if(min_triangle_layer!=std::numeric_limits<float>::max())
            RenderTriangles(std::min({min_texture_layer,min_point_layer,min_line_layer}));
    }
}

void Renderer::RenderTextures(bool post_processing,float max_layer){
    PROFILE_FUNCTION();
    
    int lastChecked=-1;
    int slot=-1;
    int lastIndex=0;

    m_Textures.VAO.Bind();
    m_Textures.VBO.Bind();
    IB.Bind();

    if(m_PostProcessingIndex!=std::numeric_limits<unsigned int>::max() && post_processing)
        PostProcessing();
    else
        m_Textures.S.Bind();

    unsigned int i;
    for(i=m_TextureIndex;i<m_Textures.NumVertices && m_BufferT[i].layer<=max_layer;i++){
        if(m_BufferT[i].texID!=lastChecked){ //new texture
            if(slot<m_MaxTextureSlots-1){ //slot available, change last id checked, increment slot and update the vertex
                lastChecked=m_BufferT[i].texID;
                ++slot;
                glActiveTexture(GL_TEXTURE0+slot);
                glBindTexture(GL_TEXTURE_2D,lastChecked);
                m_BufferT[i].texID=(float)slot;
            }else{ //no slots available
                m_Textures.VBO.SetData(0,(float *)&m_BufferT[lastIndex],i-lastIndex,sizeof(Vertex)); //send the data to the vertex buffer
                glDrawElements(GL_TRIANGLES,(i-lastIndex)/4*6,GL_UNSIGNED_INT,nullptr); //draw
                DRAW_CALLS++;
                m_TextureIndex=i; //update starting point for the next batch
                lastChecked=m_BufferT[i].texID;
                slot=0;
                m_BufferT[i].texID=(float)slot;
            }
        }else{ //same texture, just update the vertex
            m_BufferT[i].texID=(float)slot;
        }
    }
    if(i-m_TextureIndex>0){ //if there are some vertices remaining, render them
        m_Textures.VBO.SetData(0,(float *)&m_BufferT[m_TextureIndex],i-m_TextureIndex,sizeof(Vertex));
        glDrawElements(GL_TRIANGLES,(i-m_TextureIndex)/4*6,GL_UNSIGNED_INT,nullptr);
        DRAW_CALLS++;
    }

    if(m_Textures.NumVertices-i==0){
        m_TextureIndex=0;
        m_Textures.NumVertices=0;
    }else
        m_TextureIndex=i;
}

void Renderer::RenderTriangles(float max_layer){
       PROFILE_FUNCTION();

    m_Triangles.VAO.Bind();
    m_Triangles.VBO.Bind();
    m_Triangles.S.Bind();

    unsigned int i;
    for(i=m_TriangleIndex;i<m_Triangles.NumVertices && m_BufferTR[i].layer<=max_layer;i++);

    if(i-m_TriangleIndex>0){
        m_Triangles.VBO.SetData(0,(float *)&m_BufferTR[m_TriangleIndex],i-m_TriangleIndex,sizeof(TriangleVertex));
        glDrawArrays(GL_TRIANGLES,0,i-m_TriangleIndex);
        DRAW_CALLS++;
    }

    if(m_Triangles.NumVertices-i==0){
        m_TriangleIndex=0;
        m_Triangles.NumVertices=0;
    }else
        m_TriangleIndex=i;
}

void Renderer::RenderPoints(float max_layer){
    PROFILE_FUNCTION();
    
    m_Points.VAO.Bind();
    m_Points.VBO.Bind();
    m_Points.S.Bind();

    unsigned int i;
    for(i=m_PointIndex;i<m_Points.NumVertices && m_BufferP[i].layer<=max_layer;i++);

    if(i-m_PointIndex>0){
        m_Points.VBO.SetData(0,(float *)&m_BufferP[m_PointIndex],i-m_PointIndex,sizeof(LinePointVertex));
        glDrawArrays(GL_POINTS,0,i-m_PointIndex);
        DRAW_CALLS++;
    }

    if(m_Points.NumVertices-i==0){
        m_PointIndex=0;
        m_Points.NumVertices=0;
    }else
        m_PointIndex=i;
}

void Renderer::RenderLines(float max_layer){
    PROFILE_FUNCTION();
    
    m_Lines.VAO.Bind();    
    m_Lines.VBO.Bind();
    m_Lines.S.Bind();

    unsigned int i;
    for(i=m_LineIndex;i<m_Lines.NumVertices && m_BufferL[i].layer<=max_layer;i++);

    if(i-m_LineIndex>0){
        m_Lines.VBO.SetData(0,(float *)&m_BufferL[m_LineIndex],i-m_LineIndex,sizeof(LinePointVertex));
        glDrawArrays(GL_LINES,0,i-m_LineIndex);
        DRAW_CALLS++;
    }

    if(m_Lines.NumVertices-i==0){
        m_LineIndex=0;
        m_Lines.NumVertices=0;
    }else
        m_LineIndex=i;
}

void Renderer::Clear(bool ambient_light) const{
    DRAW_CALLS=0;

    if(ambient_light)
        glClearColor(m_AmbientLight.r,m_AmbientLight.g,m_AmbientLight.b,1.0f);
    else    
        glClearColor(m_ClearColor.r,m_ClearColor.g,m_ClearColor.b,1.0f);
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
    segments[0]=std::make_pair(Vec2(0,0),Vec2(0,Window::MAX_HEIGHT));
    segments[1]=std::make_pair(Vec2(0,Window::MAX_HEIGHT),Vec2(Window::MAX_WIDTH,Window::MAX_HEIGHT));
    segments[2]=std::make_pair(Vec2(Window::MAX_WIDTH,Window::MAX_HEIGHT),Vec2(Window::MAX_WIDTH,0));
    segments[3]=std::make_pair(Vec2(Window::MAX_WIDTH,0),Vec2(0,0)); 
}

void Renderer::ClearSegments(){
    segments.resize(4);
    UpdateScreenSegments();
}

std::vector<std::pair<Vec2,Vec2>> &Renderer::GetSegments(){
    return segments;
}

std::pair<Vec2,float> Renderer::GetIntersection(const std::pair<Vec2,Vec2>&ray,const std::pair<Vec2,Vec2>&seg){
    PROFILE_FUNCTION();
    
    float r_px=ray.first.x;
    float r_py=ray.first.y;
    float r_dx=ray.second.x-ray.first.x;
    float r_dy=ray.second.y-ray.first.y;

    float s_px=seg.first.x;
    float s_py=seg.first.y;
    float s_dx=seg.second.x-seg.first.x;
    float s_dy=seg.second.y-seg.first.y;

    float r_mag=glm::sqrt(r_dx*r_dx+r_dy*r_dy);
    float s_mag=glm::sqrt(s_dx*s_dx+s_dy*s_dy);
    if(r_dx/r_mag==s_dx/s_mag && r_dy/r_mag==s_dy/s_mag){
        return std::make_pair(Vec2(-1.0f,-1.0f),-1.0f); 
    }

    float T2=(r_dx*(s_py-r_py)+r_dy*(r_px-s_px))/(s_dx*r_dy-s_dy*r_dx);
    float T1=(s_px+s_dx*T2-r_px)/r_dx;

    if(T1<0) return std::make_pair(Vec2(-1.0f,-1.0f),-1.0f);
    if(T2<0 || T2>1) return std::make_pair(Vec2(-1.0f,-1.0f),-1.0f);

    return std::make_pair(Vec2(r_px+r_dx*T1,r_py+r_dy*T1),T1);
}

void Renderer::DrawLight(Vec2 pos,Vec4 color,LightType light_type,float radius,float blurAmount){
    PROFILE_FUNCTION();
    
    if(light_type==ALL_LIGHT || light_type==LIGHT_AROUND_POS_COLL){
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

            //DrawLine(a.x,a.y,b.x,b.y,0.5f,1.0f,1.0f,1.0f);
        }
        std::vector<float>angles;

        for(auto p:points){
            float angle=glm::atan(p.y-pos.y,p.x-pos.x);
            angles.push_back(angle-0.0001f);
            angles.push_back(angle);
            angles.push_back(angle+0.0001f);
        }

        std::vector<std::pair<Vec2,float>>intersects;
        for(auto angle:angles){
            float dx=glm::cos(angle);
            float dy=glm::sin(angle);

            std::pair<Vec2,Vec2>ray=std::make_pair(pos,Vec2(pos.x+dx,pos.y+dy));
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

        std::sort(begin(intersects),end(intersects),[](const auto& a,const auto& b){
            return a.second<b.second;
        });

        Render();

        if(light_type==ALL_LIGHT){
            m_LightingFramebuffer->Bind();
            glBlendFunc(GL_ONE,GL_ONE);
        }else{
            m_TempFramebuffer->Bind();
            glClearColor(0.0f,0.0f,0.0f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        for(size_t i=0;i<intersects.size()-1;i++){
            auto a=intersects[i].first;
            auto b=intersects[i+1].first;
            DrawTriangle(pos,{a.x,a.y},{b.x,b.y},color,0);
        }
        DrawTriangle(pos,intersects[0].first,intersects[intersects.size()-1].first,color,0);
        RenderTriangles(std::numeric_limits<float>::max());

        if(light_type==LIGHT_AROUND_POS_COLL){
            KeepCircle(pos,radius,blurAmount);
        }
    }else{
        Render();
        float previousSize=GetPointSize();
        SetPointSize(radius*2);

        m_Points.S.Bind();
        m_Points.S.SetUniform1f("blurAmount",blurAmount);
        m_LightingFramebuffer->Bind();
        glBlendFunc(GL_ONE,GL_ONE);

        DrawPoint(pos,color,0);
        RenderPoints(std::numeric_limits<float>::max());
        m_Points.S.SetUniform1f("blurAmount",0.0f);
        SetPointSize(previousSize);
    }
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::KeepCircle(Vec2 pos,float radius,float blurAmount){
    PROFILE_FUNCTION();
    
    m_Lights.VBO.Bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_TempFramebuffer->GetColorbufferID());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,m_LightingFramebuffer->GetColorbufferID());

    m_LightingFramebuffer->Bind();
    m_Lights.VAO.Bind();
    m_Lights.S.Bind();

    m_Lights.S.SetUniform2f("lightPos",pos.x,pos.y);
    m_Lights.S.SetUniform1f("radius",radius/Window::MAX_WIDTH*Window::Width);
    m_Lights.S.SetUniform1f("blurAmount",blurAmount);

    int sub_id=Shader::GetSubroutineIndex("KeepCircle",m_Lights.S.getID());
    Shader::SetSubroutineUniform(sub_id);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);
}

void Renderer::ApplyLight(){
    PROFILE_FUNCTION();
    
    m_Lights.VBO.Bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_Framebuffer->GetColorbufferID());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,m_LightingFramebuffer->GetColorbufferID());
    
    m_Framebuffer->Bind();
    m_Lights.VAO.Bind();
    m_Lights.S.Bind();

    int sub_id=Shader::GetSubroutineIndex("Merge",m_Lights.S.getID());
    Shader::SetSubroutineUniform(sub_id);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);
}

void Renderer::ImGui_Init(){
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Window::Window,true);
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

    Window::FPS=1.0/Window::DeltaTime;
    ImGui::Text("FPS: %.1f",Window::FPS);
    ImGui::Text("Frame Time: %.4f",Window::DeltaTime*1000);

    if(ImGui::Checkbox("V-Sync",&Window::IsVSync))
        glfwSwapInterval(Window::IsVSync);
    
    if(ImGui::Checkbox("Fullscreen",&ISFULLSCREEN))
        Window::ToggleFullScreen();
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