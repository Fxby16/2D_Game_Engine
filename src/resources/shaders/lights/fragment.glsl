#version 460 core

uniform sampler2D framebuffer;
uniform sampler2D light;

in vec2 v_TexCoord;
out vec4 FragColor;

uniform vec2 lightPos;
uniform float x_offset;
uniform float y_offset;
uniform float zoom;
uniform float radius;
uniform float blurAmount;

subroutine void Light();
subroutine uniform Light light_sub;

subroutine(Light)
void Merge(){ //merge main framebuffer and lighting framebuffer (colors get multiplied)
    vec4 color1=texture(framebuffer,v_TexCoord);
    vec4 color2=texture(light,v_TexCoord);
    FragColor=color1*color2;
}

subroutine(Light)
void KeepCircle(){ //merge lighting framebuffer and temp framebuffer. used to render point light with walls collisions
    vec4 color1=texture(framebuffer,v_TexCoord);
    vec4 color2=texture(light,v_TexCoord);

    vec2 centerCoord=gl_FragCoord.xy-vec2(lightPos.x*zoom+x_offset*zoom,lightPos.y*zoom+y_offset*zoom);
    float dist=length(centerCoord);

    if(dist>radius*zoom){
        FragColor=color2;
    }else{
        float fade=1.0-smoothstep(1.0-blurAmount,1.0,dist/(radius*zoom));
        FragColor=vec4(color1.rgb*fade,1.0)+color2;
    }
}

void main(){
    light_sub();
}