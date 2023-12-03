#version 460 core

layout(location = 0) out vec4 out_color_;

in vec4 out_color;

uniform float blurAmount;

void main() {
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    float dist = dot(circCoord, circCoord);
    if (dist > 1.0) {
        discard;
    }
    float fade = 1.0 - smoothstep(1.0 - blurAmount, 1.0, dist);
    out_color_ = vec4(out_color.rgb * fade, out_color.a);
}