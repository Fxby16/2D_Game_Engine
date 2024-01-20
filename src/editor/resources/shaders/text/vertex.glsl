#version 460 core
layout(location=0) in vec2 vertex;

out VS_OUT{
    vec2 TexCoords;
    flat int index;
}outputs;

uniform mat4 transforms[400];
uniform mat4 u_PM;

void main(){
    gl_Position=u_PM*transforms[gl_InstanceID]*vec4(vertex.xy,0.0,1.0);
    outputs.index=gl_InstanceID;
    outputs.TexCoords=vertex.xy;
    outputs.TexCoords.y=1.0f-outputs.TexCoords.y;
}