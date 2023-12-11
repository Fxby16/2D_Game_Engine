#pragma once

#include <texture.hpp>
#include <renderer.hpp>
#include <cmath>
#include <vector>
#include <structs.hpp>

class Entity{
public:
    Entity(const char *path,float x,float y,float w,float h,bool pixelart=false);
    void SetSize(int w,int h);
    void SetTexture(const char *path,bool pixelart=false);
    void Draw(Renderer &renderer);
    void Move(int x_offset,int y_offset,float frame_time);
    void AddCollider(bool insert=false);

    inline int GetX(){ return m_R.pos.x; }
    inline int GetY(){ return m_R.pos.y; }
    inline int GetW(){ return m_R.size.w; }
    inline int GetH(){ return m_R.size.h; }

private:
    Rect m_R;
    int m_Depth;
    Texture m_T;
    bool m_Collider;
};

namespace CollisionDetection{
    bool RayVsRect(const Vec2 &ray_origin, const Vec2 &ray_dir,const Rect *target,Vec2 &contact_point,Vec2 &contact_normal,float& t_hit_near);

    bool DynamicRectVsRect(const Rect* r_dynamic,const float fTimeStep,const Rect& r_static,
        Vec2 &contact_point, Vec2 &contact_normal, float& contact_time);

    bool ResolveDynamicRectVsRect(Rect* r_dynamic,const float fTimeStep,Rect* r_static);

    extern std::vector<Rect>COLLIDERS;
}