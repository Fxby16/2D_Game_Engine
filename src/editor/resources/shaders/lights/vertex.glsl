#version 460 core

layout(location=0) in vec4 position;
layout(location=1) in vec2 texCoord;

out vec2 v_TexCoord;

uniform mat4 u_PM;

void main(){
    gl_Position=u_PM*position;
    v_TexCoord=texCoord;
}