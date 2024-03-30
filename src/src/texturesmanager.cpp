#include <pch.hpp>

#include <texturesmanager.hpp>

std::pair<uint32_t,std::shared_ptr<Texture>> TexturesManager::GetTexture(const std::string &path,int mag_filter,int min_filter){
    for(auto &[id,texture]:m_Textures){
        Texture &tex=*texture.texture;

        if(tex.m_MagFilter==mag_filter && tex.m_MinFilter==min_filter && tex.m_FilePath==path){
            texture.copies++;
            return {id,texture.texture};
        }
    }

    uint32_t id=m_NextID++;
    std::shared_ptr<Texture> tex=std::make_shared<Texture>(path,mag_filter,min_filter,id);

    m_Textures[id]=TextureInfo(tex);
    return {id,tex};
}

std::shared_ptr<Texture> TexturesManager::GetTexture(uint32_t id){
    auto it=m_Textures.find(id);

    if(it==m_Textures.end()){
        return nullptr;
    }

    return it->second.texture;
}

std::pair<uint32_t,std::shared_ptr<SpriteSheet>> TexturesManager::GetSpriteSheet(const std::string &path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter){
    for(auto &[id,texture]:m_SpriteSheets){
        SpriteSheet &tex=*texture.texture;

        if(tex.m_MagFilter==mag_filter && tex.m_MinFilter==min_filter && tex.m_TileWidth==tile_width && tex.m_TileHeight==tile_height && tex.m_FilePath==path){
            texture.copies++;
            return {id,texture.texture};
        }
    }

    uint32_t id=m_NextID++;
    std::shared_ptr<SpriteSheet> tex=std::make_shared<SpriteSheet>(path,tile_width,tile_height,mag_filter,min_filter,id);

    m_SpriteSheets[id]=SpriteSheetInfo(tex);
    return {id,tex};
}

std::shared_ptr<SpriteSheet> TexturesManager::GetSpriteSheet(uint32_t id){
    auto it=m_SpriteSheets.find(id);

    if(it==m_SpriteSheets.end()){
        return nullptr;
    }

    return it->second.texture;
}

void TexturesManager::ReleaseTexture(uint32_t texture_id){
    auto it=m_Textures.find(texture_id);

    if(it==m_Textures.end()){
        return;
    }

    if(--(it->second.copies)==0){
        m_Textures.erase(it);
    }
}

void TexturesManager::ReleaseSpriteSheet(uint32_t texture_id){
    auto it=m_SpriteSheets.find(texture_id);

    if(it==m_SpriteSheets.end()){
        return;
    }

    if(--(it->second.copies)==0){
        m_SpriteSheets.erase(it);
    }
}

std::pair<uint32_t,std::shared_ptr<Texture>> TexturesManager::UpdateTexture(uint32_t texture_id,const std::string &path,int mag_filter,int min_filter){
    ReleaseTexture(texture_id);

    return GetTexture(path,mag_filter,min_filter);
}

std::pair<uint32_t,std::shared_ptr<SpriteSheet>> TexturesManager::UpdateSpriteSheet(uint32_t texture_id,const std::string &path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter){
    ReleaseSpriteSheet(texture_id);

    return GetSpriteSheet(path,tile_width,tile_height,mag_filter,min_filter);
}