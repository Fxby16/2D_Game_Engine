#version 460 core

uniform sampler2D framebuffer;
uniform sampler2D light;

in vec2 v_TexCoord;
out vec4 FragColor;

subroutine void Light();
subroutine uniform Light light_sub;

subroutine(Light)
void Merge(){
    vec4 color1=texture(framebuffer,v_TexCoord);
    vec4 color2=texture(light,v_TexCoord);
    FragColor=color1*color2;
}

void main(){
    light_sub();
}