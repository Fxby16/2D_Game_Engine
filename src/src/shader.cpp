#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <shader.hpp>

Shader::Shader(const char *VertexShaderPath,const char *FragmentShaderPath){
    ID=glCreateProgram();

std::string vertexCode;
std::string fragmentCode;
std::ifstream vertexFile;
std::ifstream fragmentFile;
    //get shader source code from file
    vertexFile.open(VertexShaderPath);
    fragmentFile.open(FragmentShaderPath);
    std::stringstream vertexStream,fragmentStream;
    vertexStream<<vertexFile.rdbuf();
    fragmentStream<<fragmentFile.rdbuf();
    vertexFile.close();
    fragmentFile.close();
    vertexCode=vertexStream.str();
    fragmentCode=fragmentStream.str();

    const char *vertexSourceCode=vertexCode.c_str();
    const char *fragmentSourceCode=fragmentCode.c_str();

    Compile(vertexSourceCode,fragmentSourceCode);
}

Shader::~Shader(){
    glDeleteProgram(ID);
}

void Shader::Bind() const{
    glUseProgram(ID);
}

void Shader::Unbind() const{
    glUseProgram(0);
}

void Shader::Compile(const char *vertexSourceCode,const char *fragmentSourceCode){
unsigned int vertexShader,fragmentShader;
    //compile vertex Shader
    vertexShader=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexSourceCode,NULL);
    glCompileShader(vertexShader);
    CheckShaderErrors(vertexShader);
    //compile fragment Shader
    fragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentSourceCode,NULL);
    glCompileShader(fragmentShader);
    CheckShaderErrors(fragmentShader);
    // shader program
    glAttachShader(ID,vertexShader);
    glAttachShader(ID,fragmentShader);
    glLinkProgram(ID);
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

bool Shader::CheckShaderErrors(GLuint ShaderID){
    GLint CompileStatus;
    glGetShaderiv(ShaderID,GL_COMPILE_STATUS,&CompileStatus);
    if(CompileStatus!=GL_TRUE){
        GLint InfoLogLength;
        glGetShaderiv(ShaderID,GL_INFO_LOG_LENGTH,&InfoLogLength);
        GLchar *buffer=new GLchar[InfoLogLength];

        GLsizei BufferSize;
        glGetShaderInfoLog(ShaderID,InfoLogLength,&BufferSize,buffer);
        std::cout<<buffer<<std::endl;
        delete [] buffer;
        return false;
    }
    return true;
}

void Shader::SetUniform4f(const std::string &name,float v0,float v1,float v2,float v3){
    glUniform4f(GetUniformLocation(name),v0,v1,v2,v3);
}

void Shader::SetUniform2f(const std::string &name,float v0,float v1){
    glUniform2f(GetUniformLocation(name),v0,v1);
}

void Shader::SetUniform1iv(const std::string &name,int *v,unsigned int num_elem){
    glUniform1iv(GetUniformLocation(name),num_elem,v);
}

void Shader::SetUniform1f(const std::string &name,float v0){
    glUniform1f(GetUniformLocation(name),v0);
}

void Shader::SetUniformMat4f(const std::string &name,glm::mat4 &proj){
    glUniformMatrix4fv(GetUniformLocation(name),1,GL_FALSE,&proj[0][0]);
}

void Shader::SetUniform1i(const std::string &name,int v0){
    glUniform1i(GetUniformLocation(name),v0);
}

int Shader::GetUniformLocation(const std::string &name){
    if(Uniforms_Cache.find(name)!=Uniforms_Cache.end())
        return Uniforms_Cache[name];
    int location=glGetUniformLocation(ID,name.c_str());
    Uniforms_Cache[name]=location;
    if(location==-1)
        std::cout<<"Warning: Uniform "<<name<<" doesn't exist"<<std::endl;
    return location;
}