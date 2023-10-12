#version 330

layout(location=0) in vec4 position;
layout(location=1) in vec4 color;

out vec4 out_color;

uniform mat4 u_PM;

void main() {
    gl_Position = u_PM*position;
    gl_PointSize = 20;

    out_color=color;
}
