#include <textrenderer.hpp>
#include <global.hpp>
#include <cstdio>
#include <memory.hpp>

TextRenderer::TextRenderer(const char *font_path):
    m_VBO(6,sizeof(float)*2,GL_STATIC_DRAW),
    m_Shader("resources/shaders/text/vertex.glsl","resources/shaders/text/fragment.glsl")
{

    m_Characters=(Character*)AllocateMemory(CH_NUM*sizeof(Character));
    m_Transforms=(glm::mat4*)AllocateMemory(CH_LIMIT*sizeof(glm::mat4));
    m_ToRender=(int*)AllocateMemory(CH_LIMIT*sizeof(int));

    if(FT_Init_FreeType(&m_FT))
        perror("FREETYPE ERROR: Couldn't init FreeType Library\n");

    if(FT_New_Face(m_FT,font_path,0,&m_Face))
        perror("FREETYPE ERROR: Failed to load font\n");
    else{
        FT_Select_Charmap(m_Face,ft_encoding_unicode);
        FT_Set_Pixel_Sizes(m_Face,256,256);
        glPixelStorei(GL_UNPACK_ALIGNMENT,1); //disable byte-alignment restriction

        glGenTextures(1,&m_TextureArrayID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY,m_TextureArrayID);
        glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_R8,256,256,CH_NUM,0,GL_RED,GL_UNSIGNED_BYTE,0);

        for(int ch=0;ch<CH_NUM;ch++){
            if(FT_Load_Char(m_Face,ch,FT_LOAD_RENDER)){
                perror("FREETYPE ERROR: Failed to load Glyph\n");
                continue;
            }

            if(m_Face->glyph->bitmap.rows>256){
                #ifdef DEBUG
                    printf("FREETYPE WARNING: Glyph %c is too tall\n",ch);
                #endif
                continue;
            }

            glTexSubImage3D(
                GL_TEXTURE_2D_ARRAY,
                0,0,0,ch,
                m_Face->glyph->bitmap.width,
                m_Face->glyph->bitmap.rows,1,
                GL_RED,GL_UNSIGNED_BYTE,
                m_Face->glyph->bitmap.buffer
            );

            glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            
            Character character={
                ch,
                glm::ivec2(m_Face->glyph->bitmap.width,m_Face->glyph->bitmap.rows),
                glm::ivec2(m_Face->glyph->bitmap_left,m_Face->glyph->bitmap_top),
                (unsigned int)m_Face->glyph->advance.x
            };
            m_Characters[ch]=character;
        }
    }
    FT_Done_Face(m_Face);
    FT_Done_FreeType(m_FT);

    float vertices[]={
        0.0f,1.0f,
        0.0f,0.0f,
        1.0f,1.0f,
        1.0f,0.0f,
    };

    m_VBO.SetData(0,vertices,4,sizeof(float)*2);
    m_VBL.Push(GL_FLOAT,2,false);
    m_VAO.AddBuffer(m_VBO,m_VBL);
}

TextRenderer::~TextRenderer(){
    FreeMemory(m_Characters);
    FreeMemory(m_Transforms);
    FreeMemory(m_ToRender);

    glDeleteTextures(1,&m_TextureArrayID);
}

void TextRenderer::DrawText(std::string text,float x,float y,float scale,Vec3 color){
    scale=scale*48.0f/256.0f;
    float copyX=x;
    m_Shader.Bind();
    m_Shader.SetUniform3f("textColor",color.r,color.g,color.b);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY,m_TextureArrayID);
    m_VAO.Bind();
    m_VBO.Bind();

    int workingIndex=0;
    for(auto c:text){
        Character ch=m_Characters[c];
        if(c=='\n'){
            y-=ch.Size.y*1.3*scale;
            x=copyX;
        }else if(c==' ')
            x+=(ch.Advance>>6)*scale;
        else{
            float xpos=x+ch.Bearing.x*scale;
            float ypos=y-(256-ch.Bearing.y)*scale;

            m_Transforms[workingIndex]=glm::translate(glm::mat4(1.0f),glm::vec3(xpos,ypos,0))*glm::scale(glm::mat4(1.0f),glm::vec3(256*scale,256*scale,0));
            m_ToRender[workingIndex]=ch.TexID;
            x+=(ch.Advance>>6)*scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
            workingIndex++;
            if(workingIndex==CH_LIMIT-1){
                Render(workingIndex);
                workingIndex=0;
            }
        }
    }
    Render(workingIndex);
}

std::pair<float,float> TextRenderer::GetTextSize(std::string text,float scale){
    scale=scale*48.0f/256.0f;
    float width_=0.0f;
    float height_=m_Characters['\n'].Size.y*1.3*scale;
    float max_width=0.0f;
    for(auto c:text){
        Character ch=m_Characters[c];
        if(c=='\n'){
            height_+=ch.Size.y*1.3*scale;
            max_width=std::max(max_width,width_);
            width_=0.0f;
        }else if(c==' ')
            width_+=(ch.Advance>>6)*scale;
        else{
            width_+=(ch.Advance>>6)*scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
    }
    return std::make_pair(std::max(max_width,width_),height_);
}

void TextRenderer::Render(int num_characters){
    if(num_characters!=0){
        m_Shader.SetUniformMat4fv("transforms",glm::value_ptr(m_Transforms[0]),num_characters);
        m_Shader.SetUniform1iv("letterMap",&m_ToRender[0],num_characters);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP,0,4,num_characters);
    }
}