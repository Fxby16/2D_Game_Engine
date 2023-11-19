#version 460 core

layout(location = 0) out vec4 out_color_;

in vec4 out_color;

void main() {
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    if (dot(circCoord, circCoord) > 1.0) {
        discard;
    }
    out_color_ = out_color;
}
