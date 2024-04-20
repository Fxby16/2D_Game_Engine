#pragma once

class Gizmo{
public:
    Gizmo(): m_MousePos(-1.0f,-1.0f){}
    Gizmo(glm::vec2 viewport_pos,glm::vec2 viewport_size): m_MousePos(-1.0f,-1.0f),m_ViewPortPos(viewport_pos),m_ViewPortSize(viewport_size){}
    ~Gizmo()=default;
    
    void Manipulate(glm::mat4 &matrix,const glm::mat4 &view,const glm::mat4 &projection);

    void SetViewPortPos(const glm::vec2 &pos){ m_ViewPortPos = pos; }
    void SetViewPortSize(const glm::vec2 &size){ m_ViewPortSize = size; }
private:
    void DrawGizmo(const glm::mat4 &matrix,bool vertical_arrow=false,bool horizontal_arrow=false);
    bool IsMouseOnArrowH(double xpos,double ypos,const std::pair<glm::vec2,glm::vec2> &arrow);
    bool IsMouseOnArrowV(double xpos,double ypos,const std::pair<glm::vec2,glm::vec2> &arrow);

    glm::vec2 m_MousePos;
    glm::vec2 m_ViewPortPos;
    glm::vec2 m_ViewPortSize;
    bool m_VerticalArrow=false;
    bool m_HorizontalArrow=false;
};