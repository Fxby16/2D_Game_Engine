#pragma once

#include <renderer.hpp>
#include <texture.hpp>

class Examples{
public:
    static void BatchRendering(Renderer &renderer,Texture &texture);
    static void Spritesheet(Renderer &renderer,SpriteSheet &spritesheet);
    static void BatchingMultipleTextures(Renderer &renderer,std::vector<Texture*>&t);
};