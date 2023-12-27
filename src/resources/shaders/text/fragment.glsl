#version 460 core

layout(location=0) out vec4 color;

in VS_OUT{
    vec2 TexCoords;
    flat int index;
}inputs;

uniform sampler2DArray text;
uniform int letterMap[400];
uniform vec3 textColor;

void main(){    
    vec4 sampled=vec4(1.0,1.0,1.0,texture(text,vec3(inputs.TexCoords.xy,letterMap[inputs.index])).r);
    color=vec4(textColor,1.0)*sampled;
}