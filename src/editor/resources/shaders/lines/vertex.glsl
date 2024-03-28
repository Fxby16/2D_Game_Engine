#version 460 core

layout(location=0) in vec4 position;
layout(location=1) in vec4 color;
layout(location=3) in int layer;

out vec4 out_color;
 
uniform mat4 u_PM;

void main(){
    gl_Position=u_PM*position;

    out_color=color;
}
