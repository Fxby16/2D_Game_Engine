#pragma once

#include <texture.hpp>

struct TextureInfo{
    unsigned int copies;
    std::shared_ptr<Texture> texture;

    TextureInfo(): copies(0),texture(nullptr){}
    TextureInfo(std::shared_ptr<Texture> tex): copies(1),texture(tex){}
};

struct SpriteSheetInfo{
    unsigned int copies;
    std::shared_ptr<SpriteSheet> texture;

    SpriteSheetInfo(): copies(0),texture(nullptr){}
    SpriteSheetInfo(std::shared_ptr<SpriteSheet> tex): copies(1),texture(tex){}
};

class TexturesManager{
public:
    TexturesManager()=default;
    ~TexturesManager()=default;

    /**
     * Returns a texture from the parameters given. If the texture is already loaded, it will return the texture and increase the number of copies,
     * otherwise it will load the texture and return it.
     * \return A pair with the texture id and the texture itself.
    */
    std::pair<uint32_t,std::shared_ptr<Texture>> GetTexture(const std::string &path,int mag_filter,int min_filter);
    
    /**
     * Returns a texture from the id given.
     * \return the texture
    */
    std::shared_ptr<Texture> GetTexture(uint32_t id);

    /**
     * Returns a spritesheet from the parameters given. If the spritesheet is already loaded, it will return the spritesheet and increase the number of copies,
     * otherwise it will load the spritesheet and return it.
     * \return A pair with the texture id and the texture itself.
    */
    std::pair<uint32_t,std::shared_ptr<SpriteSheet>> GetSpriteSheet(const std::string &path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter);

    /**
     * Returns a spritesheet from the id given.
     * \return the spritesheet
    */
    std::shared_ptr<SpriteSheet> GetSpriteSheet(uint32_t id);

    /**
     * Decreases the number of copies of the texture with the id given. If the number of copies reaches 0, the texture will be deleted.
    */
    void ReleaseTexture(uint32_t texture_id);

    /**
     * Decreases the number of copies of the spritesheet with the id given. If the number of copies reaches 0, the spritesheet will be deleted.
    */
    void ReleaseSpriteSheet(uint32_t texture_id);

    /**
     * Updates the texture with the id given with the new parameters.
    */
    std::pair<uint32_t,std::shared_ptr<Texture>> UpdateTexture(uint32_t texture_id,const std::string &path,int mag_filter,int min_filter);

    /**
     * Updates the spritesheet with the id given with the new parameters.
    */
    std::pair<uint32_t,std::shared_ptr<SpriteSheet>> UpdateSpriteSheet(uint32_t texture_id,const std::string &path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter);

private:
    std::unordered_map<uint32_t,struct TextureInfo> m_Textures;
    std::unordered_map<uint32_t,struct SpriteSheetInfo> m_SpriteSheets;

    uint32_t m_NextID=0;
};