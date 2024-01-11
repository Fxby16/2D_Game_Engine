#include <camera.hpp>
#include <window.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Instrumentor.h>

Camera::Camera(){
    m_Position=Vec2(0.0f,0.0f);
    m_Zoom=1.0f;
    m_Rotation=0.0f;
}

void Camera::InitializeProj(){
    PROFILE_FUNCTION();

    m_Proj=glm::ortho(0.0f,Window::MAX_WIDTH,0.0f,Window::MAX_HEIGHT,-1.0f,1.0f);
    m_View=glm::translate(glm::mat4(1.0f),glm::vec3(-m_Position.x,-m_Position.y,0.0f));
    m_View=glm::rotate(m_View,glm::radians(m_Rotation),glm::vec3(0.0f,0.0f,1.0f));
    m_View=glm::scale(m_View,glm::vec3(m_Zoom,m_Zoom,1.0f));

    m_ViewProj=m_Proj*m_View;

    RENDERER->m_Points.S.Bind();
    RENDERER->m_Points.S.SetUniformMat4fv("u_PM",glm::value_ptr(m_ViewProj),1);
    RENDERER->m_Points.S.SetUniform1f("zoom",m_Zoom);

    RENDERER->m_Lines.S.Bind();
    RENDERER->m_Lines.S.SetUniformMat4fv("u_PM",glm::value_ptr(m_ViewProj),1);

    RENDERER->m_Textures.S.Bind();
    RENDERER->m_Textures.S.SetUniformMat4fv("u_PM",glm::value_ptr(m_ViewProj),1);
    
    RENDERER->m_Triangles.S.Bind();
    RENDERER->m_Triangles.S.SetUniformMat4fv("u_PM",glm::value_ptr(m_ViewProj),1);

    RENDERER->m_Lights.S.Bind();
    RENDERER->m_Lights.S.SetUniformMat4fv("view_matrix",glm::value_ptr(m_View),1);
    RENDERER->m_Lights.S.SetUniformMat4fv("u_PM",glm::value_ptr(m_Proj),1);
    RENDERER->m_Lights.S.SetUniform1f("window_width",Window::Width);
    RENDERER->m_Lights.S.SetUniform1f("zoom",m_Zoom);

    RENDERER->m_SPostProcessing.Bind();
    RENDERER->m_SPostProcessing.SetUniformMat4fv("u_PM",glm::value_ptr(m_ViewProj),1);

    RENDERER->m_Zoom=m_Zoom;

    for(int i=0;i<TEXT_RENDERERS.size();i++){
        if(TEXT_RENDERERS[i]->m_Fixed)    
            TEXT_RENDERERS[i]->UpdateProjMat(m_Proj);
        else{
            TEXT_RENDERERS[i]->UpdateProjMat(m_ViewProj);
        }
    }
}

void Camera::DrawSceneProj(){
    PROFILE_FUNCTION();
    
    RENDERER->m_Textures.S.Bind();
    RENDERER->m_Textures.S.SetUniformMat4fv("u_PM",glm::value_ptr(m_Proj),1);
}

void Camera::ResetSceneProj(){
    PROFILE_FUNCTION();
    
    RENDERER->m_Textures.S.Bind();
    RENDERER->m_Textures.S.SetUniformMat4fv("u_PM",glm::value_ptr(m_ViewProj),1);
}

void Camera::SetPosition(Vec2 pos){
    PROFILE_FUNCTION();
    
    m_Position=pos;
    InitializeProj();
}

void Camera::Move(float x_offset,float y_offset){
    PROFILE_FUNCTION();
    
    m_Position.x+=x_offset;
    m_Position.y+=y_offset;
    InitializeProj();
}

void Camera::SetZoom(float zoom){
    PROFILE_FUNCTION();
    
    m_Zoom=zoom;
    m_Zoom=glm::clamp(m_Zoom,0.1f,3.0f);
    RENDERER->m_Zoom=m_Zoom;
    RENDERER->UpdateLineWidth();

    InitializeProj();
}

void Camera::Zoom(float zoom_offset){
    PROFILE_FUNCTION();
    
    m_Zoom+=zoom_offset;
    m_Zoom=glm::clamp(m_Zoom,0.1f,3.0f);
    RENDERER->m_Zoom=m_Zoom;
    RENDERER->UpdateLineWidth();

    InitializeProj();
}

void Camera::SetRotation(float rotation){
    PROFILE_FUNCTION();
    
    m_Rotation=rotation;
    InitializeProj();
}

void Camera::Rotate(float rotation_offset){
    PROFILE_FUNCTION();
    
    m_Rotation+=rotation_offset;
    InitializeProj();
}