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
    Vec2 Position;
    Vec2 TexCoords;
    float Depth;
    float TexID;
};

struct LinePointVertex{
    Vec2 Pos;
    Vec4 Color;
};

struct TriangleVertex{
    Vec2 Position;
    Vec4 Color;
    TriangleVertex();
    TriangleVertex(Vec2 pos,Vec4 color);
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
};