#include <pch.hpp>
#include <fontmanager.hpp>

std::pair<uint32_t,std::shared_ptr<TextRenderer>> FontManager::GetFont(const std::string &path,int glyph_size,bool fixed){
    for(auto &[id,font]:m_Fonts){
        TextRenderer &f=*font.font;

        if(f.m_LoadedGlyphSize==glyph_size && f.m_Fixed==fixed && strcmp(f.m_LoadedFontPath.c_str(),path.c_str())==0){
            font.copies++;
            return {id,font.font};
        }
    }

    uint32_t id=m_NextID++;
    std::shared_ptr<TextRenderer> f=std::make_shared<TextRenderer>(path,glyph_size,fixed,id);

    m_Fonts[id]=FontInfo(f);
    return {id,f};
}

std::shared_ptr<TextRenderer> FontManager::GetFont(uint32_t id){
    auto it=m_Fonts.find(id);

    if(it==m_Fonts.end())
        return nullptr;

    return it->second.font;
}

void FontManager::ReleaseFont(uint32_t font_id){
    auto it=m_Fonts.find(font_id);

    if(it==m_Fonts.end())
        return;

    if(--(it->second.copies)==0)
        m_Fonts.erase(it);
}

std::pair<uint32_t,std::shared_ptr<TextRenderer>> FontManager::UpdateFont(uint32_t font_id,const std::string &path,int glyph_size,bool fixed){
    ReleaseFont(font_id);

    return GetFont(path,glyph_size,fixed);
}