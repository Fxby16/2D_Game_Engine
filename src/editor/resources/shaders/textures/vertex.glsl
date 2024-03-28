#version 460 core

layout(location=0) in vec4 position;
layout(location=1) in vec2 texCoord;
layout(location=2) in int layer;
layout(location=3) in float T_Index;

out vec2 v_TexCoord;
out float Tex_Index;

uniform mat4 u_PM;

void main(){
    Tex_Index=T_Index;
    gl_Position=u_PM*position;
    v_TexCoord=texCoord;
}