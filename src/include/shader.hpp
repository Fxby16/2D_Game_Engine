#pragma once

class Shader{
public:
   
    Shader(const char *vertex_shader_path,const char *fragment_shader_path);
    Shader()=default;
    ~Shader();
    void Load(const char *vertex_shader_path,const char *fragment_shader_path);
    void Unload();
    void Reload();

    void Bind() const;
    void Unbind() const;
    inline unsigned int getID(){ return m_ID; }

    void SetUniform4f(const std::string &name,float v0,float v1,float v2,float v3);
    void SetUniform3f(const std::string &name,float v0,float v1,float v2);
    void SetUniform2f(const std::string &name,float v0,float v1);
    void SetUniform1f(const std::string &name,float v0);
    void SetUniformMat4fv(const std::string &name,float *proj,unsigned int num_elem);
    void SetUniform1iv(const std::string &name,int *v,unsigned int num_elem);
    void SetUniform1i(const std::string &name,int v0);
    static void SetSubroutineUniform(unsigned int uniform_index);
    static unsigned int GetSubroutineIndex(const char *uniform_name,unsigned int shader_id);

private:
    std::unordered_map<std::string,int>m_UniformsCache;
    unsigned int m_ID;

    bool m_Loaded=false;

    char *m_VertexPath=nullptr;
    char *m_FragmentPath=nullptr;

    void Compile(const char *vertex_src_code,const char *fragment_src_code);
    bool CheckShaderErrors(GLuint shader_ID);
    int GetUniformLocation(const std::string &name);
};