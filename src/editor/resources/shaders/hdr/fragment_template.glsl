#version 460 core

layout(location=0) out vec4 color;

in vec2 v_TexCoord;
uniform sampler2D tex;

uniform float gamma;
uniform float exposure;

vec3 reinhardToneMapping(vec3 hdr_color) {
    vec3 mapped = hdr_color / (hdr_color + vec3(1.0));
    return mapped;
}

vec3 filmicToneMapping(vec3 hdr_color) {
    vec3 x = max(vec3(0.0), hdr_color - 0.004);
    vec3 mapped = (x*(6.2*x + .5)) / (x*(6.2*x + 1.7) + 0.06);
    return mapped;
}

vec3 uncharted2ToneMapping(vec3 hdr_color) {
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    vec3 mapped = ((hdr_color*(A*hdr_color+C*B)+D*E)/(hdr_color*(A*hdr_color+B)+D*F))-E/F;
    return mapped;
}

vec3 acesToneMapping(vec3 hdr_color) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    vec3 mapped = clamp((hdr_color*(a*hdr_color+b))/(hdr_color*(c*hdr_color+d)+e), 0.0, 1.0);
    return mapped;
}

vec3 exponentialToneMapping(vec3 hdr_color) {
    vec3 mapped = vec3(1.0) - exp(-hdr_color);
    mapped = pow(mapped, vec3(1.0 / gamma));
    return mapped;
}

vec3 logarithmicToneMapping(vec3 hdr_color) {
    vec3 mapped = log2(vec3(1.0) + hdr_color);
    mapped = pow(mapped, vec3(1.0 / gamma));
    return mapped;
}

vec3 mantiukToneMapping(vec3 hdr_color) {
    float L = 0.2126 * hdr_color.r + 0.7152 * hdr_color.g + 0.0722 * hdr_color.b;
    vec3 mapped = hdr_color * (1.0 + (L / pow(1.0, 2.0))) / (1.0 + L);
    return mapped;
}