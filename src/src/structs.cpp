#include <structs.hpp>

Vec2::Vec2(): x(0),y(0){}
Vec2::Vec2(float x,float y): x(x),y(y){}

Vec3::Vec3(): r(0),g(0),b(0){}
Vec3::Vec3(float r,float g,float b): r(r),g(g),b(b){}

Vec4::Vec4(): r(0),g(0),b(0),a(0){}
Vec4::Vec4(float r,float g,float b,float a): r(r),g(g),b(b),a(a){}

TriangleVertex::TriangleVertex(): position(Vec2()),color(Vec4()){}
TriangleVertex::TriangleVertex(Vec2 pos,Vec4 color): position(pos),color(color){}

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