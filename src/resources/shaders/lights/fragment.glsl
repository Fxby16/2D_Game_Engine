#version 460 core

uniform sampler2D framebuffer;
uniform sampler2D light;

in vec2 v_TexCoord;
out vec4 FragColor;

uniform vec2 lightPos;
uniform float radius;
uniform float blurAmount;

subroutine void Light();
subroutine uniform Light light_sub;

subroutine(Light)
void Merge(){
    vec4 color1=texture(framebuffer,v_TexCoord);
    vec4 color2=texture(light,v_TexCoord);
    FragColor=color1*color2;
}

subroutine(Light)
void KeepCircle(){
    vec4 color1=texture(framebuffer,v_TexCoord);
    vec4 color2=texture(light,v_TexCoord);

    // Calcola le coordinate del pixel rispetto al centro
    vec2 centerCoord = gl_FragCoord.xy - lightPos;

    // Calcola la distanza dal centro
    float dist = length(centerCoord);

    // Controlla se il pixel è al di fuori del cerchio
    if (dist > radius) {
        FragColor = color2;
    } else {
        // Calcola l'effetto di sfocatura basato sulla distanza
        float fade = 1.0 - smoothstep(1.0 - blurAmount, 1.0, dist / radius);

        // Applica l'effetto di sfocatura al colore di output
        FragColor = vec4(color1.rgb*fade,1.0)+color2;
    }
}

void main(){
    light_sub();
}