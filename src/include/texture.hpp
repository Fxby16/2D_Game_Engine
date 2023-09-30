#pragma once

#include <string>
#include <array>

#include <vertexbuffer.hpp>
#include <glad/glad.h>

class Texture{
private:
    unsigned int ID;
    std::string filepath;
    unsigned char *local_buffer;
    int width,height,BPP;
public:
    /**
     * create a 2d texture from the given image
     * \param path the path to the image
    */
    Texture(const std::string &path,GLint mag_filter,GLint min_filter);
    ~Texture();

    void Bind(unsigned int slot=0) const;
    void Unbind() const;

    inline int GetWidth() const{ return width; }
    inline int GetHeight() const{ return height; }
};

class SpriteSheet{
private:
    Texture t;
    int tile_width;
    int tile_height;
public:
    SpriteSheet(const std::string &path,unsigned int tile_width,unsigned int tile_height,GLint mag_filter,GLint min_filter)
    :t(path,mag_filter,min_filter),tile_width(tile_width),tile_height(tile_height){}

    void Bind(unsigned int slot=0) const;
    void Unbind() const;

    inline int GetTileWidth() const{ return tile_width; }
    inline int GetTileHeight() const{ return tile_height; }
    inline int GetWidth() const{ return t.GetWidth(); }
    inline int GetHeight() const{ return t.GetHeight(); }
};