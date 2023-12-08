#include <glad/glad.h>
#include <string>
#include <cstdio>

#include <shader.hpp>

Shader::Shader(const char *vertex_shader_path,const char *fragment_shader_path){
    m_ID=glCreateProgram();

    std::string vertexCode;
    std::string fragmentCode;
    FILE *vertexFile=fopen(vertex_shader_path,"r");
    FILE *fragmentFile=fopen(fragment_shader_path,"r");

    if(vertexFile==NULL){
        perror("Couldn't open vertex shader file\n");
        return;
    }

    if(fragmentFile==NULL){
        perror("Couldn't open fragment shader file\n");
        return;
    }
    
    char ch;
    while((ch=fgetc(vertexFile))!=EOF){
        vertexCode+=ch;
    }
    while((ch=fgetc(fragmentFile))!=EOF){
        fragmentCode+=ch;
    }

    fclose(vertexFile);
    fclose(fragmentFile);

    const char *vertexSourceCode=vertexCode.c_str();
    const char *fragmentSourceCode=fragmentCode.c_str();

    Compile(vertexSourceCode,fragmentSourceCode);
}

Shader::~Shader(){
    glDeleteProgram(m_ID);
}

void Shader::Bind() const{
    glUseProgram(m_ID);
}

void Shader::Unbind() const{
    glUseProgram(0);
}

void Shader::Compile(const char *vertex_src_code,const char *fragment_src_code){
unsigned int vertexShader,fragmentShader;

    //compile vertex Shader
    vertexShader=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertex_src_code,NULL);
    glCompileShader(vertexShader);
    CheckShaderErrors(vertexShader);

    //compile fragment Shader
    fragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragment_src_code,NULL);
    glCompileShader(fragmentShader);
    CheckShaderErrors(fragmentShader);
    
    glAttachShader(m_ID,vertexShader);
    glAttachShader(m_ID,fragmentShader);
    glLinkProgram(m_ID);
    
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
        puts(buffer);
        delete []buffer;
        return false;
    }
    return true;
}

void Shader::SetUniform4f(const std::string &name,float v0,float v1,float v2,float v3){
    glUniform4f(GetUniformLocation(name),v0,v1,v2,v3);
}

void Shader::SetUniform3f(const std::string &name,float v0,float v1,float v2){
    glUniform3f(GetUniformLocation(name),v0,v1,v2);
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
    if(m_UniformsCache.find(name)!=m_UniformsCache.end())
        return m_UniformsCache[name];
    int location=glGetUniformLocation(m_ID,name.c_str());
    m_UniformsCache[name]=location;
    if(location==-1)
        printf("Warning: Uniform %s doesn't exist\n",name.c_str());
    return location;
}

unsigned int Shader::GetSubroutineIndex(const char *uniform_name,unsigned int shader_id){
    unsigned int temp;
    if(uniform_name==nullptr){
        return std::numeric_limits<unsigned int>::max();
    }
    temp=glGetSubroutineIndex(shader_id,GL_FRAGMENT_SHADER,uniform_name);
    return temp;
}

void Shader::SetSubroutineUniform(unsigned int uniform_index){
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER,1,&uniform_index);
}