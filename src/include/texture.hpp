#pragma once

#include <string>

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
    Texture(const std::string &path);
    ~Texture();

    void Bind(unsigned int slot=0) const;
    void Unbind() const;

    inline int GetWidth() const{ return width; }
    inline int GetHeight() const{ return height; }
};