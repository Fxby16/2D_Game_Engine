#pragma once

struct Vec2{
    union{
        float x;
        float w;
    };
    union{
        float y;
        float h;
    };
    Vec2();
    Vec2(float x,float y);
};

struct Vec3{
    float r,g,b;
    Vec3();
    Vec3(float r,float g,float b);
};

struct Vec4{
    float r,g,b,a;
    Vec4();
    Vec4(float r,float g,float b,float a);
};

struct Vertex{
    Vec2 position;
    Vec2 texcoords;
    int layer;
    float texID;

    Vertex();
    Vertex(Vec2 pos,Vec2 texcoords,int layer,float texID);
};

struct LineVertex{
    Vec2 pos;
    Vec4 color;
    int layer;

    LineVertex();
    LineVertex(Vec2 pos,Vec4 color,int layer);
};

struct PointVertex{
    Vec2 pos;
    Vec4 color;
    float border;
    int layer;

    PointVertex();
    PointVertex(Vec2 pos,Vec4 color,float border,int layer);
};

struct TriangleVertex{
    Vec2 position;
    Vec4 color;
    int layer;
    TriangleVertex();
    TriangleVertex(Vec2 pos,Vec4 color,int layer);
};

Vec2 operator+(Vec2 a,Vec2 b);
Vec2 operator-(Vec2 a,Vec2 b);
Vec2 operator*(Vec2 a,Vec2 b);
Vec2 operator*(float a,Vec2 b);
Vec2 operator*(Vec2 a,float b);
Vec2 operator/(Vec2 a,Vec2 b);
Vec2 operator/(float a,Vec2 b);
Vec2 operator/(Vec2 a,int b);

struct Rect{
    Vec2 pos;
    Vec2 size;
    Vec2 vel;

    Rect();
    Rect(Vec2 pos,Vec2 size,Vec2 vel);
};

std::ostream &operator<<(std::ostream &os,const Vec2 &v);
std::ostream &operator<<(std::ostream &os,const Vec3 &v);
std::ostream &operator<<(std::ostream &os,const Vec4 &v);
std::ostream &operator<<(std::ostream &os,const Vertex &v);
std::ostream &operator<<(std::ostream &os,const LineVertex &v);
std::ostream &operator<<(std::ostream &os,const PointVertex &v);
std::ostream &operator<<(std::ostream &os,const TriangleVertex &v);
std::ostream &operator<<(std::ostream &os,const Rect &v);