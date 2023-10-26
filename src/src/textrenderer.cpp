#include <textrenderer.hpp>
#include <global.hpp>
#include <cstdio>

TextRenderer::TextRenderer(const char *font_path):
    VBO(6,sizeof(float)*2,GL_STATIC_DRAW),
    shader("resources/shaders/text/vertex.glsl","resources/shaders/text/fragment.glsl"),
    transforms(CH_LIMIT),to_render(CH_LIMIT)
{
    UpdateProjMat();
    shader.Bind();
    shader.SetUniformMat4f("u_PM",proj);

    if(FT_Init_FreeType(&ft))
        fprintf(stderr,"FREETYPE ERROR: Couldn't init FreeType Library\n");

    if(FT_New_Face(ft,font_path,0,&face))
        fprintf(stderr,"FREETYPE ERROR: Failed to load font\n");
    else{
        FT_Select_Charmap(face,ft_encoding_unicode);
        FT_Set_Pixel_Sizes(face,256,256);
        glPixelStorei(GL_UNPACK_ALIGNMENT,1); //disable byte-alignment restriction

        glGenTextures(1,&textureArrayID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY,textureArrayID);
        glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_R8,256,256,CH_NUM,0,GL_RED,GL_UNSIGNED_BYTE,0);

        for(uint8_t ch=0;ch<CH_NUM;ch++){
            if(FT_Load_Char(face,ch,FT_LOAD_RENDER)){
                fprintf(stderr,"FREETYPE ERROR: Failed to load Glyph\n");
                continue;
            }
            glTexSubImage3D(
                GL_TEXTURE_2D_ARRAY,
                0,0,0,ch,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,1,
                GL_RED,GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );

            glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            
            Character character={
                ch,
                glm::ivec2(face->glyph->bitmap.width,face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left,face->glyph->bitmap_top),
                (unsigned int)face->glyph->advance.x
            };
            characters[ch]=character;
        }
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    float vertices[]={
        0.0f,1.0f,
        0.0f,0.0f,
        1.0f,1.0f,
        1.0f,0.0f,
    };

    VBO.SetData(0,vertices,4,sizeof(float)*2);
    VBL.Push(GL_FLOAT,2,false);
    VAO.AddBuffer(VBO,VBL);
}

TextRenderer::~TextRenderer(){
    glDeleteTextures(1,&textureArrayID);
}

void TextRenderer::RenderText(std::string text,float x,float y,float scale,float *color){
    scale=scale*48.0f/256.0f;
    float copyX=x;
    shader.Bind();
    shader.SetUniform3f("textColor",color[0],color[1],color[2]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY,textureArrayID);
    VAO.Bind();
    VBO.Bind();

    int workingIndex=0;
    for(auto c:text){
        Character ch=characters[c];
        if(c=='\n'){
            y-=((ch.size_.y))*1.3*scale;
            x=copyX;
        }else if(c==' ')
            x+=(ch.advance>>6)*scale;
        else{
            float xpos=x+ch.bearing.x*scale;
            float ypos=y-(256-ch.bearing.y)*scale;

            transforms[workingIndex]=glm::translate(glm::mat4(1.0f),glm::vec3(xpos,ypos,0))*glm::scale(glm::mat4(1.0f),glm::vec3(256*scale,256*scale,0));
            to_render[workingIndex]=ch.texID;
            x+=(ch.advance>>6)*scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
            workingIndex++;
            if(workingIndex==CH_LIMIT-1){
                Draw(workingIndex);
                workingIndex=0;
            }
        }
    }
    Draw(workingIndex);
}

void TextRenderer::Draw(int num_characters){
    if(num_characters!=0){
        glUniformMatrix4fv(shader.GetUniformLocation("transforms"),num_characters,GL_FALSE,&transforms[0][0][0]);
        shader.SetUniform1iv("letterMap",&to_render[0],num_characters);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP,0,4,num_characters);
    }
}