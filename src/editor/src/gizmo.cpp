#include <pch.hpp>
#include <gizmo.hpp>
#include <renderer.hpp>
#include <window.hpp>

#define ARROW_WIDTH 0.05f
#define ARROW_LENGTH 0.8f
#define TRIANGLE_SIZE 0.1f

void Gizmo::Manipulate(glm::mat4 &matrix,const glm::mat4 &view,const glm::mat4 &projection){
    if(glfwGetMouseButton(Window::Window,GLFW_MOUSE_BUTTON_LEFT)!=GLFW_PRESS){
        m_MousePos.x=-1.0f;
        m_MousePos.y=-1.0f;
        m_VerticalArrow=false;
        m_HorizontalArrow=false;

        DrawGizmo(matrix);
        return;   
    }

    double xpos,ypos;
    glfwGetCursorPos(Window::Window,&xpos,&ypos);

    ypos=Window::Height-ypos;

    glm::mat4 translation=view*matrix;

    float translated_x=translation[3][0];
    float translated_y=translation[3][1];

    std::pair<glm::vec2,glm::vec2> arrow_right=std::make_pair(glm::vec2(translated_x,translated_y),glm::vec2(translated_x+ARROW_LENGTH,translated_y));
    std::pair<glm::vec2,glm::vec2> arrow_up=std::make_pair(glm::vec2(translated_x,translated_y),glm::vec2(translated_x,translated_y+ARROW_LENGTH));

    if(!(xpos>=m_ViewPortPos.x && xpos<=m_ViewPortPos.x+m_ViewPortSize.x &&
       ypos<=m_ViewPortPos.y && ypos>=m_ViewPortPos.y-m_ViewPortSize.y)){

        m_MousePos.x=-1.0f;
        m_MousePos.y=-1.0f;
        m_VerticalArrow=false;
        m_HorizontalArrow=false;

        DrawGizmo(matrix);
        return;
    }

    bool vertical_arrow=IsMouseOnArrowV((xpos-m_ViewPortPos.x)/m_ViewPortSize.x*Window::MAX_WIDTH,(ypos-(m_ViewPortPos.y-m_ViewPortSize.y))/m_ViewPortSize.x*Window::MAX_WIDTH,arrow_up);
    bool horizontal_arrow=IsMouseOnArrowH((xpos-m_ViewPortPos.x)/m_ViewPortSize.x*Window::MAX_WIDTH,(ypos-(m_ViewPortPos.y-m_ViewPortSize.y))/m_ViewPortSize.x*Window::MAX_WIDTH,arrow_right);

    if((!vertical_arrow && !horizontal_arrow) && m_MousePos.x==-1.0f && m_MousePos.y==-1.0f){
        m_MousePos.x=-1.0f;
        m_MousePos.y=-1.0f;
        m_VerticalArrow=false;
        m_HorizontalArrow=false;

        DrawGizmo(matrix);
        return;
    }

    if(!m_VerticalArrow && !m_HorizontalArrow){
        m_VerticalArrow=vertical_arrow;
        m_HorizontalArrow=horizontal_arrow;
    }

    xpos-=m_ViewPortPos.x;
    ypos-=(Window::Height-m_ViewPortPos.y);

    if(m_MousePos.x!=-1.0f && m_MousePos.y!=-1.0f){
        if(m_VerticalArrow){
            matrix=matrix*glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,(ypos-m_MousePos.y)/m_ViewPortSize.x*Window::MAX_WIDTH,0.0f));
            DrawGizmo(matrix,true);
        }else if(m_HorizontalArrow){
            matrix=matrix*glm::translate(glm::mat4(1.0f),glm::vec3((xpos-m_MousePos.x)/m_ViewPortSize.x*Window::MAX_WIDTH,0.0f,0.0f));
            DrawGizmo(matrix,false,true);
        }else{
            DrawGizmo(matrix);
        }
    }

    m_MousePos.x=xpos;
    m_MousePos.y=ypos;
}

void Gizmo::DrawGizmo(const glm::mat4 &matrix,bool vertical_arrow,bool horizontal_arrow){
    glm::vec3 translation=glm::vec3(matrix[3]);

    float x=translation.x;
    float y=translation.y;

    float line_width=RENDERER->GetLineWidth();
    RENDERER->SetLineWidth(ARROW_WIDTH);

    Vec4 horizontal_arrow_color=Vec4(1.0f,0.0f,0.0f,1.0f);
    Vec4 vertical_arrow_color=Vec4(0.0f,0.0f,1.0f,1.0f);

    if(horizontal_arrow){
        horizontal_arrow_color=Vec4(1.0f,0.4f,0.4f,1.0f);
    }else if(vertical_arrow){
        vertical_arrow_color=Vec4(0.4f,0.4f,1.0f,1.0f);
    }

    RENDERER->DrawLine(Vec2(x,y),Vec2(x+ARROW_LENGTH,y),horizontal_arrow_color,0);
    RENDERER->DrawLine(Vec2(x,y),Vec2(x,y+ARROW_LENGTH),vertical_arrow_color,0);

    RENDERER->DrawTriangle(Vec2(x+ARROW_LENGTH+TRIANGLE_SIZE,y),Vec2(x+ARROW_LENGTH,y+TRIANGLE_SIZE),Vec2(x+ARROW_LENGTH,y-TRIANGLE_SIZE),horizontal_arrow_color,0);
    RENDERER->DrawTriangle(Vec2(x,y+ARROW_LENGTH+TRIANGLE_SIZE),Vec2(x+TRIANGLE_SIZE,y+ARROW_LENGTH),Vec2(x-TRIANGLE_SIZE,y+ARROW_LENGTH),vertical_arrow_color,0);

    RENDERER->Render();

    RENDERER->SetLineWidth(line_width);
}

bool Gizmo::IsMouseOnArrowH(double xpos,double ypos,const std::pair<glm::vec2,glm::vec2> &arrow){
    if(xpos>=arrow.first.x && xpos<=arrow.second.x &&
       ypos>=arrow.first.y-ARROW_WIDTH && ypos<=arrow.first.y+ARROW_WIDTH){
        return true;
    }

    return false;
}

bool Gizmo::IsMouseOnArrowV(double xpos,double ypos,const std::pair<glm::vec2,glm::vec2> &arrow){
    if(xpos>=arrow.first.x-ARROW_WIDTH && xpos<=arrow.first.x+ARROW_WIDTH &&
       ypos>=arrow.first.y && ypos<=arrow.second.y){
        return true;
    }

    return false;
}