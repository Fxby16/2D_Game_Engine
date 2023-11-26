#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

class Shader{
private:
    std::unordered_map<std::string,int>m_UniformsCache;
    unsigned int m_ID;
public:
   
    Shader(const char *vertex_shader_path,const char *fragment_shader_path);
    ~Shader();
    void Bind() const;
    void Unbind() const;
    inline unsigned int getID(){ return m_ID; }

    void Compile(const char *vertex_src_code,const char *fragment_src_code);

    bool CheckShaderErrors(GLuint shader_ID);

    void SetUniform4f(const std::string &name,float v0,float v1,float v2,float v3);
    void SetUniform3f(const std::string &name,float v0,float v1,float v2);
    void SetUniform2f(const std::string &name,float v0,float v1);
    void SetUniform1f(const std::string &name,float v0);
    void SetUniformMat4f(const std::string &name,glm::mat4 &proj);
    void SetUniform1iv(const std::string &name,int *v,unsigned int num_elem);
    void SetUniform1i(const std::string &name,int v0);
    int GetUniformLocation(const std::string &name);
    static unsigned int GetSubroutineIndex(const char *uniform_name,unsigned int shader_id);
    static void SetSubroutineUniform(unsigned int uniform_index);
};