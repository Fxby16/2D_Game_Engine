#version 460 core

uniform sampler2D framebuffer;
uniform sampler2D light;

in vec2 v_TexCoord;
out vec4 FragColor;

uniform vec2 lightPos;
uniform float zoom;
uniform float radius;
uniform float blurAmount;

uniform mat4 view_matrix;
uniform float window_width;

subroutine void Light();
subroutine uniform Light light_sub;

subroutine(Light)
void Merge(){ //merge main framebuffer and lighting framebuffer (colors get multiplied)
    vec4 color1=vec4(texture(framebuffer,v_TexCoord).rgb,1.0);
    vec4 color2=vec4(texture(light,v_TexCoord).rgb,1.0);
    FragColor=color1*color2;
}

subroutine(Light)
void KeepCircle(){ //merge lighting framebuffer and temp framebuffer. used to render point light with walls collisions
    vec4 color1=texture(framebuffer,v_TexCoord);
    vec4 color2=texture(light,v_TexCoord);

    vec2 newLightCoords=(view_matrix*vec4(lightPos.x,lightPos.y,0.0,1.0)).xy;
    newLightCoords=vec2(newLightCoords.x/10.0*window_width,newLightCoords.y/10.0*window_width);

    vec2 centerCoord=gl_FragCoord.xy-newLightCoords;
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