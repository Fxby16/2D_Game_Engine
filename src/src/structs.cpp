#include <pch.hpp>
#include <structs.hpp>

Vec2::Vec2(): x(0),y(0){}
Vec2::Vec2(float x,float y): x(x),y(y){}

Vec3::Vec3(): r(0),g(0),b(0){}
Vec3::Vec3(float r,float g,float b): r(r),g(g),b(b){}

Vec4::Vec4(): r(0),g(0),b(0),a(0){}
Vec4::Vec4(float r,float g,float b,float a): r(r),g(g),b(b),a(a){}

TriangleVertex::TriangleVertex(): position(Vec2()),color(Vec4()),layer(0){}
TriangleVertex::TriangleVertex(Vec2 pos,Vec4 color,int layer): position(pos),color(color),layer(layer){}

LineVertex::LineVertex(): pos(Vec2()),color(Vec4()),layer(0){}
LineVertex::LineVertex(Vec2 pos,Vec4 color,int layer): pos(pos),color(color),layer(layer){}

PointVertex::PointVertex(): pos(Vec2()),color(Vec4()),layer(0){}
PointVertex::PointVertex(Vec2 pos,Vec4 color,float border,int layer): pos(pos),color(color),border(border),layer(layer){}

Vertex::Vertex(): position(Vec2()),texcoords(Vec2()),layer(0),texID(0){}
Vertex::Vertex(Vec2 pos,Vec2 texcoords,int layer,float texID): position(pos),texcoords(texcoords),layer(layer),texID(texID){}

Vec2 operator+(Vec2 a,Vec2 b){
    return {a.x+b.x,a.y+b.y};
}

Vec2 operator-(Vec2 a,Vec2 b){
    return {a.x-b.x,a.y-b.y};
}

Vec2 operator*(Vec2 a,Vec2 b){
    return {a.x*b.x,a.y*b.y};
}

Vec2 operator*(float a,Vec2 b){
    return {a*b.x,a*b.y};
}

Vec2 operator*(Vec2 a,float b){
    return {a.x*b,a.y*b};
}

Vec2 operator/(Vec2 a,Vec2 b){
    return {a.x/b.x,a.y/b.y};
}

Vec2 operator/(float a,Vec2 b){
    return {a/b.x,a/b.y};
}

Vec2 operator/(Vec2 a,int b){
    return {a.x/b,a.y/b};
}

Rect::Rect(): pos(Vec2()),size(Vec2()),vel(Vec2()){}
Rect::Rect(Vec2 pos,Vec2 size,Vec2 vel): pos(pos),size(size),vel(vel){}

std::ostream &operator<<(std::ostream &os,const Vec2 &v){
    os<<"["<<v.x<<","<<v.y<< "]";
    return os;
}

std::ostream &operator<<(std::ostream &os,const Vec3 &v){
    os<<"["<<v.r<<","<<v.g<<","<<v.b<< "]";
    return os;
}

std::ostream &operator<<(std::ostream &os,const Vec4 &v){
    os<<"["<<v.r<<","<<v.g<<","<<v.b<<","<<v.a<< "]";
    return os;
}

std::ostream &operator<<(std::ostream &os,const Vertex &v){
    os<<"Position: "<<v.position<<" Texcoords: "<<v.texcoords<<" Layer: "<<v.layer<<" TexID: "<<v.texID;
    return os;
}

std::ostream &operator<<(std::ostream &os,const LineVertex &v){
    os<<"Position: "<<v.pos<<" Color: "<<v.color<<" Layer: "<<v.layer;
    return os;
}

std::ostream &operator<<(std::ostream &os,const PointVertex &v){
    os<<"Position: "<<v.pos<<" Color: "<<v.color<<" Border: "<<v.border<<" Layer: "<<v.layer;
    return os;
}

std::ostream &operator<<(std::ostream &os,const TriangleVertex &v){
    os<<"Position: "<<v.position<<" Color: "<<v.color<<" Layer: "<<v.layer;
    return os;
}

std::ostream &operator<<(std::ostream &os,const Rect &v){
    os<<"Position: "<<v.pos<<" Size: "<<v.size<<" Velocity: "<<v.vel;
    return os;
}