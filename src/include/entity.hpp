#pragma once

#include <texture.hpp>
#include <renderer.hpp>
#include <cmath>
#include <vector>

struct Vec2D{
    union{
        float x;
        float w;
    };
    union{
        float y;
        float h;
    };
    Vec2D(): x(0),y(0){}
    Vec2D(float x,float y): x(x),y(y){}
};

Vec2D operator+(Vec2D a,Vec2D b);
Vec2D operator-(Vec2D a,Vec2D b);
Vec2D operator*(Vec2D a,Vec2D b);
Vec2D operator*(float a,Vec2D b);
Vec2D operator*(Vec2D a,float b);
Vec2D operator/(Vec2D a,Vec2D b);
Vec2D operator/(float a,Vec2D b);
Vec2D operator/(Vec2D a,int b);

struct Rect{
    Vec2D pos;
    Vec2D size;
    Vec2D vel;
};

class Entity{
private:
    Rect m_R;
    int m_Depth;
    Texture m_T;
    bool m_Collider;
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
};

namespace CollisionDetection{
    bool RayVsRect(const Vec2D &ray_origin, const Vec2D &ray_dir,const Rect *target,Vec2D &contact_point,Vec2D &contact_normal,float& t_hit_near);

    bool DynamicRectVsRect(const Rect* r_dynamic,const float fTimeStep,const Rect& r_static,
        Vec2D &contact_point, Vec2D &contact_normal, float& contact_time);

    bool ResolveDynamicRectVsRect(Rect* r_dynamic,const float fTimeStep,Rect* r_static);

    extern std::vector<Rect>COLLIDERS;
}