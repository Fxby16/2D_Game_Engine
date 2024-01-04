#pragma once

#include <structs.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera{
public:
    Camera();
    ~Camera()=default;

    void InitializeProj();
    void DrawSceneProj();
    void ResetSceneProj();  

    void SetPosition(Vec2 pos);
    void Move(float x_offset,float y_offset);
    void SetZoom(float zoom);
    void Zoom(float zoom_offset);
    /**
     * \param rotation Angle in degrees
    */
    void SetRotation(float rotation);
    /**
     * \param rotation_offset Angle offset in degrees
    */
    void Rotate(float rotation_offset);

private:
    Vec2 m_Position;
    float m_Zoom;
    float m_Rotation;

    glm::mat4 m_View;
    glm::mat4 m_Proj;
};