#version 460 core

uniform sampler2D framebuffer;
uniform sampler2D light;
uniform float xpos;
uniform float ypos;

in vec2 v_TexCoord;
out vec4 FragColor;

uniform float edgeWidth;
uniform float radius;

subroutine void Light();
subroutine uniform Light light_sub;

float smoothEdge(float radius, float edgeWidth, vec2 position) {
    return smoothstep(radius + edgeWidth,radius, length(position));
}

subroutine(Light)
void CircleAroundPos(){
    vec4 color1=texture(framebuffer,v_TexCoord);
    vec4 color2=texture(light,v_TexCoord);
    
    vec2 center = vec2(xpos,ypos); // center of the circle

    vec2 position = (v_TexCoord - center)/vec2(1.0,16.0/9.0);
    if (length(position) > radius) {
        float edgeFactor = smoothEdge(radius, edgeWidth, position);
        FragColor = mix(vec4(0.3,0.3,0.3,1.0)*color1, color1, edgeFactor);
    }else{
        FragColor=color1;
    }
}

subroutine(Light)
void CircleAroundPosColl(){ //only difference with CircleAroundPos is that this uses also light framebuffer
    vec4 color1=texture(framebuffer,v_TexCoord);
    vec4 color2=texture(light,v_TexCoord);
    
    vec2 center = vec2(xpos,ypos); // center of the circle

    vec2 position = (v_TexCoord - center)/vec2(1.0,16.0/9.0);
    if (length(position) > radius) {
        float edgeFactor = smoothEdge(radius, edgeWidth, position);
        FragColor = mix(vec4(0.3,0.3,0.3,1.0)*color1, color1*color2, edgeFactor);
    }else{
        FragColor=color1*color2;
    }
}

subroutine(Light)
void AllLight(){
    vec4 color1=texture(framebuffer,v_TexCoord);
    vec4 color2=texture(light,v_TexCoord);
    FragColor=color1*color2;
}

void main(){
    light_sub();
}