#pragma once

#include <renderer.hpp>
#include <textrenderer.hpp>
#include <audio.hpp>
#include <texture.hpp>
#include <entity.hpp>

namespace Examples{
    void BatchRendering(Renderer &renderer,Texture &texture);
    void Spritesheet(Renderer &renderer,SpriteSheet &spritesheet);
    void BatchingMultipleTextures(Renderer &renderer,std::vector<Texture*>&t);
    void DepthTest(Renderer &renderer,Texture &t1,Texture &t2);
    void LinesPoints(Renderer &renderer);
    void PostProcessing(Renderer &renderer,Texture &t);
    void Text(TextRenderer &textrenderer);
    void Sounds(AudioPlayer &audioplayer);
    void Entities(Renderer &renderer,Entity &e0,Entity &e1,Entity &e2,Entity &e3,Entity &e4);
};