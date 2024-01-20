#version 460 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in float Tex_Index;
uniform sampler2D texID[32];

subroutine void PostProcessing();
subroutine uniform PostProcessing post_processing;

subroutine(PostProcessing) 
void InvertColor(){
    color=vec4(1.0,1.0,1.0,1.0)-texture(texID[int(Tex_Index)],v_TexCoord);
    color.a=1.0;
}

subroutine(PostProcessing) 
void GreenChannel(){
    color=texture(texID[int(Tex_Index)],v_TexCoord); //keeping only green channel 
    color.r=0.0;
    color.b=0.0;
}

subroutine(PostProcessing)
void GrayScale(){
    color=texture(texID[int(Tex_Index)],v_TexCoord);
    float temp=(0.3*color.r)+(0.59*color.g)+(0.11*color.b);
    color=vec4(temp,temp,temp,1.0);
}

subroutine(PostProcessing)
void DarkerImg(){
    vec4 tmp=texture(texID[int(Tex_Index)],v_TexCoord);
    color=vec4(0.3*tmp.r,0.3*tmp.g,0.3*tmp.b,1.0);
}

void main(){
    post_processing();
}