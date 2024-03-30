#pragma once

#include <textrenderer.hpp>

struct FontInfo{
    unsigned int copies;
    std::shared_ptr<TextRenderer> font;

    FontInfo(): copies(0),font(nullptr){}
    FontInfo(std::shared_ptr<TextRenderer> f): copies(1),font(f){}
};

class FontManager{
public:
    FontManager()=default;
    ~FontManager()=default;

    /**
     * Returns a font from the parameters given. If the font is already loaded, it will return the font and increase the number of copies,
     * otherwise it will load the font and return it.
     * \return A pair with the font id and the font itself.
    */
    std::pair<uint32_t,std::shared_ptr<TextRenderer>> GetFont(const std::string &path,int glyph_size,bool fixed);

    /**
     * Returns a font from the id given.
     * \return the font
    */
    std::shared_ptr<TextRenderer> GetFont(uint32_t id);

    /**
     * Decreases the number of copies of the font with the id given. If the number of copies reaches 0, the font will be deleted.
    */
    void ReleaseFont(uint32_t font_id);

    /**
     * Updates the font with the id given with the new parameters.
    */
    std::pair<uint32_t,std::shared_ptr<TextRenderer>> UpdateFont(uint32_t font_id,const std::string &path,int glyph_size,bool fixed);

    inline std::unordered_map<uint32_t,struct FontInfo> &GetFonts(){
        return m_Fonts;
    }
private:
    std::unordered_map<uint32_t,struct FontInfo> m_Fonts;

    uint32_t m_NextID=0;
};