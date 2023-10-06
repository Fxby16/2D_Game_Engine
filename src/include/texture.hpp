#pragma once

#include <string>
#include <array>

#include <vertexbuffer.hpp>
#include <glad/glad.h>

class Texture{
protected:
    unsigned int ID;
    std::string filepath;
    unsigned char *local_buffer;
    int width,height,BPP;
public:
    Texture(){}
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
    inline unsigned int GetTexID() const{ return ID; }
};

class SpriteSheet : public Texture{
private:
    int tile_width;
    int tile_height;
public:
    SpriteSheet(const std::string &path,unsigned int tile_width,unsigned int tile_height,GLint mag_filter,GLint min_filter)
    :Texture(path,mag_filter,min_filter),tile_width(tile_width),tile_height(tile_height){}

    inline int GetTileWidth() const{ return tile_width; }
    inline int GetTileHeight() const{ return tile_height; }

    std::array<Vertex,4> CreateQuadSpriteSheet(float x,float y,float row,float col,float scale,float depth,float texID);
};