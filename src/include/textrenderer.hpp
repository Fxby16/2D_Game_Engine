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

namespace Window{
    extern float Width,Height;
    extern const float MAX_WIDTH;
    extern float MAX_HEIGHT;
}

inline constexpr unsigned int CH_LIMIT=400;
inline constexpr unsigned int CH_NUM=128;

class TextRenderer{
public:
    /**
     * \param fixed false if its position should be relative to the camera
    */
    TextRenderer(const char *font_path,float glyph_size,bool fixed);
    ~TextRenderer();

    void DrawText(std::string text,float x,float y,float scale,Vec3 color);
    std::pair<float,float> GetTextSize(std::string text,float scale); 

    inline void UpdateProjMat(glm::mat4 &proj){
        m_Shader.Bind();
        m_Shader.SetUniformMat4fv("u_PM",glm::value_ptr(proj),1);
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

    unsigned int m_TextureArrayID;
    Character *m_Characters;
    glm::mat4 *m_Transforms;
    int *m_ToRender;

    float m_GlyphSize;

    void Render(int num_characters);  

public:
    bool m_Fixed;
};