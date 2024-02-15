#pragma once

#include <vertexbuffer.hpp>
#include <vertexbufferlayout.hpp>
#include <vertexarray.hpp>
#include <shader.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

inline constexpr unsigned int CH_LIMIT=400;
inline constexpr unsigned int CH_NUM=128;

class TextRenderer{
public:
    /**
     * \param fixed false if its position should be relative to the camera
    */
    TextRenderer(const char *font_path,float glyph_size,bool fixed);
    ~TextRenderer();

    /**
     * Draw the text.
     * Coordinates are expected in engine units (screen width = 10 units)
     * \param text a string containing the text
     * \param x the x position of the text
     * \param y the y position of the text
     * \param scale the scale of the text
     * \param color the color of the text
    */
    void DrawText(std::string text,float x,float y,float scale,Vec3 color);
    /**
     * Get the size of the text.
     * \param text a string containing the text
     * \param scale the scale of the text
    */
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