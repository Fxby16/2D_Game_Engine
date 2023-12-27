#pragma once

#include <vertexbuffer.hpp>
#include <vertexbufferlayout.hpp>
#include <vertexarray.hpp>
#include <shader.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

inline constexpr unsigned int CH_LIMIT=400;
inline constexpr unsigned int CH_NUM=128;

class TextRenderer{
public:
    TextRenderer(const char *font_path);
    ~TextRenderer();

    void DrawText(std::string text,float x,float y,float scale,Vec3 color);
    std::pair<float,float> GetTextSize(std::string text,float scale); 

    inline void UpdateProjMat(){
        m_Proj=glm::ortho(0.0f,(float)SCREEN_WIDTH,0.0f,(float)SCREEN_HEIGHT,-1.0f,1.0f);
        m_Shader.Bind();
        m_Shader.SetUniformMat4fv("u_PM",glm::value_ptr(m_Proj),1);
    }

private:

    struct Character{
        unsigned int TexID;     // ID of the glyph texture       
        glm::ivec2 Size;       // Size of glyph
        glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
        unsigned int Advance;   // Horizontal offset to advance to next glyph
    };

    FT_Library m_FT;
    FT_Face m_Face;

    VertexBuffer m_VBO;
    VertexArray m_VAO;
    VertexBufferLayout m_VBL;
    Shader m_Shader;
    glm::mat4 m_Proj;

    unsigned int m_TextureArrayID;
    Character *m_Characters;
    glm::mat4 *m_Transforms;
    int *m_ToRender;

    void Render(int num_characters);  
};