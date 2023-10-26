#pragma once

#include <vertexbuffer.hpp>
#include <vertexbufferlayout.hpp>
#include <vertexarray.hpp>
#include <shader.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

inline constexpr unsigned int CH_LIMIT=400;
inline constexpr unsigned int CH_NUM=128;

class TextRenderer{
private:

    struct Character{
        unsigned int texID;     // ID of the glyph texture       
        glm::ivec2 size_;       // Size of glyph
        glm::ivec2 bearing;     // Offset from baseline to left/top of glyph
        unsigned int advance;   // Horizontal offset to advance to next glyph
    };

    FT_Library ft;
    FT_Face face;

    VertexBuffer VBO;
    VertexArray VAO;
    VertexBufferLayout VBL;
    Shader shader;
    glm::mat4 proj;

    unsigned int textureArrayID;
    Character characters[CH_NUM];
    std::vector<glm::mat4> transforms;
    std::vector<int> to_render;
public:
    TextRenderer(const char *font_path);
    ~TextRenderer();

    inline void UpdateProjMat(){
        proj=glm::ortho(0.0f,(float)SCREEN_WIDTH,0.0f,(float)SCREEN_HEIGHT,-1.0f,1.0f);
    }

    void RenderText(std::string text,float x,float y,float scale,float *color);
    void Draw(int num_characters);
};