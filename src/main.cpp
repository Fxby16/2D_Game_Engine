#include <game.hpp>

#include <vector>
#include <cmath>
#include <algorithm>

class Example : public Game{
private:
    std::vector<std::pair<Vec2D,Vec2D>>segments;
    bool show_points,show_lines,show_light;
    Texture ciccio;
public:
    Example(const char *window_name): Game(window_name),show_points(false),show_lines(false),show_light(true),ciccio("resources/textures/cicciogamer89.jpg",GL_LINEAR,GL_LINEAR){
        m_Renderer->ChangePointSize(3);
        m_Renderer->ChangeLineWidth(2);
        
        segments.push_back(std::make_pair(Vec2D(0,0),Vec2D(0,SCREEN_HEIGHT)));
        segments.push_back(std::make_pair(Vec2D(0,SCREEN_HEIGHT),Vec2D(SCREEN_WIDTH,SCREEN_HEIGHT)));
        segments.push_back(std::make_pair(Vec2D(SCREEN_WIDTH,SCREEN_HEIGHT),Vec2D(SCREEN_WIDTH,0)));
        segments.push_back(std::make_pair(Vec2D(SCREEN_WIDTH,0),Vec2D(0,0)));
        
        segments.push_back(std::make_pair(Vec2D(300,200),Vec2D(700,200)));
        segments.push_back(std::make_pair(Vec2D(700,200),Vec2D(700,500)));
        segments.push_back(std::make_pair(Vec2D(700,500),Vec2D(300,500)));
        segments.push_back(std::make_pair(Vec2D(300,500),Vec2D(300,200)));

        segments.push_back(std::make_pair(Vec2D(100,200),Vec2D(200,200)));
        segments.push_back(std::make_pair(Vec2D(200,200),Vec2D(200,300)));
        segments.push_back(std::make_pair(Vec2D(200,300),Vec2D(100,300)));
        segments.push_back(std::make_pair(Vec2D(100,300),Vec2D(100,200)));

        segments.push_back(std::make_pair(Vec2D(1100,600),Vec2D(1100,100)));
        segments.push_back(std::make_pair(Vec2D(1100,100),Vec2D(1000,100)));
        segments.push_back(std::make_pair(Vec2D(1000,100),Vec2D(1000,600)));
        segments.push_back(std::make_pair(Vec2D(1000,600),Vec2D(1100,600)));
    }

    void OnUpdate(double frame_time) override{
        m_Renderer->Clear({0,0,0});

        m_Renderer->DrawTexture(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,1,0,ciccio.GetTexID());

        segments[0]=std::make_pair(Vec2D(0,0),Vec2D(0,SCREEN_HEIGHT));
        segments[1]=std::make_pair(Vec2D(0,SCREEN_HEIGHT),Vec2D(SCREEN_WIDTH,SCREEN_HEIGHT));
        segments[2]=std::make_pair(Vec2D(SCREEN_WIDTH,SCREEN_HEIGHT),Vec2D(SCREEN_WIDTH,0));
        segments[3]=std::make_pair(Vec2D(SCREEN_WIDTH,0),Vec2D(0,0));

        for(auto [a,b]:segments)
            m_Renderer->DrawLine(a.x,a.y,b.x,b.y,1,0,0,1);

        std::vector<Vec2D>points;
        auto find=[&points](Vec2D point)->bool{
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
        }
        std::vector<float>angles;

        double xpos, ypos;
        glfwGetCursorPos(WINDOW, &xpos, &ypos);
        ypos=SCREEN_HEIGHT-ypos;

        for(auto p:points){
            float angle=std::atan2(p.y-ypos,p.x-xpos);
            angles.push_back(angle-0.0001f);
            angles.push_back(angle);
            angles.push_back(angle+0.0001f);
        }

        auto GetIntersection=[](std::pair<Vec2D,Vec2D>ray,std::pair<Vec2D,Vec2D>seg)->std::pair<Vec2D,float>{
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
                return std::make_pair(Vec2D(-1.0f,-1.0f),-1.0f); // Unit vectors are the same.
            }

            float T2=(r_dx*(s_py-r_py)+r_dy*(r_px-s_px))/(s_dx*r_dy-s_dy*r_dx);
	        float T1=(s_px+s_dx*T2-r_px)/r_dx;
        
            if(T1<0) return std::make_pair(Vec2D(-1.0f,-1.0f),-1.0f);
	        if(T2<0 || T2>1) return std::make_pair(Vec2D(-1.0f,-1.0f),-1.0f);
        
            return std::make_pair(Vec2D(r_px+r_dx*T1,r_py+r_dy*T1),T1);
        };

        std::vector<std::pair<Vec2D,float>>intersects;
        for(auto angle:angles){
            float dx=std::cos(angle);
            float dy=std::sin(angle);

            std::pair<Vec2D,Vec2D>ray=std::make_pair(Vec2D(xpos,ypos),Vec2D(xpos+dx,ypos+dy));
            std::pair<Vec2D,double> closest_intersect=std::make_pair(Vec2D(-1.0f,-1.0f),-1.0f);
            for(auto seg:segments){
                std::pair<Vec2D,double> intersect=GetIntersection(ray,seg);
                if(intersect.first.x==-1.0f && intersect.first.y==-1.0f)
                    continue;
                if((closest_intersect.first.x==-1.0f && closest_intersect.first.y==-1.0f) || (intersect.second<closest_intersect.second && intersect.second!=-1.0f))
                    closest_intersect=intersect;
            }
            if(closest_intersect.first.x!=-1.0f && closest_intersect.first.y!=-1.0f)
                intersects.push_back(std::make_pair(closest_intersect.first,angle));
        }

        for(auto p:intersects){
            if(show_lines)
                m_Renderer->DrawLine(p.first.x,p.first.y,xpos,ypos,0,1,0,1);
            if(show_points)
                m_Renderer->DrawPoint(p.first.x,p.first.y,0,0,1,1);
        }

        m_Renderer->Render();
        
        if(show_light){
            std::sort(begin(intersects),end(intersects),[](const auto& a,const auto& b){
                return a.second<b.second;
            });

            m_Renderer->BindLightingFB();
            m_Renderer->Clear({0.3f,0.3f,0.3f});

            for(size_t i=0;i<intersects.size()-1;i++){
                auto a=intersects[i].first;
                auto b=intersects[i+1].first;
                m_Renderer->DrawTriangle(xpos,ypos,a.x,a.y,b.x,b.y,1,1,1,1);
            }
            m_Renderer->DrawTriangle(xpos,ypos,intersects[0].first.x,intersects[0].first.y,intersects[intersects.size()-1].first.x,intersects[intersects.size()-1].first.y,1,1,1,1);
            m_Renderer->RenderTriangles();
            m_Renderer->ApplyLight("CircleAroundPos",xpos,ypos);
        }
    }

    void OnImGuiUpdate() override{
        Renderer::ImGui_Content();

        ImGui::SetNextWindowSize(ImVec2(0,0));
        ImGui::Begin("Options",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::Checkbox("Show Points",&show_points);
        ImGui::Checkbox("Show Lines",&show_lines);
        ImGui::Checkbox("Show Light",&show_light);
        ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH/2-ImGui::GetWindowWidth()/2,0));
        ImGui::End();
    }
};

int main(){
    Example *example=new Example("Test");
    example->Run();
    delete example;

    return 0;
}