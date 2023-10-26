#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

class Shader{
private:
    std::unordered_map<std::string,int>Uniforms_Cache;
    unsigned int ID;
public:
    /**
     * \param VertexShaderPath path to the file that contains the vertex shader
     * \param FragmentShaderPath path to the file that contains the fragment shader
    */
    Shader(const char *VertexShaderPath,const char *FragmentShaderPath);
    ~Shader();
    void Bind() const;
    void Unbind() const;
    inline unsigned int getID(){ return ID; }
    /**
     * \param vertexSourceCode a string containing the vertex shader
     * \param fragmentSourceCode a string containing the fragment shader
    */
    void Compile(const char *vertexSourceCode,const char *fragmentSourceCode);

    bool CheckShaderErrors(GLuint ShaderID);

    void SetUniform4f(const std::string &name,float v0,float v1,float v2,float v3);
    void SetUniform3f(const std::string &name,float v0,float v1,float v2);
    void SetUniform2f(const std::string &name,float v0,float v1);
    void SetUniform1f(const std::string &name,float v0);
    void SetUniformMat4f(const std::string &name,glm::mat4 &proj);
    void SetUniform1iv(const std::string &name,int *v,unsigned int num_elem);
    void SetUniform1i(const std::string &name,int v0);
    int GetUniformLocation(const std::string &name);
};