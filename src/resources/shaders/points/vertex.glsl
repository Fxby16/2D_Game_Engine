#version 460 core

layout(location=0) in vec4 position;
layout(location=1) in vec4 color;
layout(location=2) in float border;
layout(location=3) in int layer;

out vec4 out_color;
out float circle_border;

uniform mat4 u_PM;
uniform float zoom;
uniform float pointSize;

void main(){
    gl_PointSize = pointSize*zoom;
    gl_Position = u_PM*position;

    out_color=color;
    circle_border=border;
}