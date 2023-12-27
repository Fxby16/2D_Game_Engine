#version 460 core

layout(location=0) out vec4 color;

in vec2 v_TexCoord;
in float Tex_Index;
uniform sampler2D texID[32];

void main(){
    int index=int(Tex_Index);
    color=texture(texID[index],v_TexCoord);
}