#pragma once

#include <scene.hpp>

class SceneSerializer{
public:
    SceneSerializer()=default;

    void SetScene(Scene *scene);

    void Serialize(const std::string &path,std::vector<std::pair<std::string,uint32_t>>&script_components);
    void SerializeEncrypted(const std::string &path,std::vector<std::pair<std::string,uint32_t>>&script_components);
    #ifdef EDITOR
    bool Deserialize(const std::string &path,std::vector<std::pair<std::string,uint32_t>>&script_components);
    bool DeserializeEncrypted(const std::string &path,std::vector<std::pair<std::string,uint32_t>>&script_components);
    bool DeserializeNode(const YAML::Node &data,std::vector<std::pair<std::string,uint32_t>>&script_components);
    #elif defined(APPLICATION)
    bool Deserialize(const std::string &path);
    bool DeserializeEncrypted(const std::string &path);
    bool DeserializeNode(const YAML::Node &data);
    #endif

private:
    Scene *m_Scene=nullptr;
};

struct TextureData{
    TextureData(uint32_t uid,float width,float height,int layer):
        uid(uid),width(width),height(height),layer(layer),animated(false){}
    TextureData(uint32_t uid,float width,float height,int layer,bool playanimation,
        bool loopanimation,float animationdelay,int animationindex): uid(uid),width(width),height(height),layer(layer),
        animated(true),playanimation(playanimation),loopanimation(loopanimation),animationdelay(animationdelay),animationindex(animationindex){}

    uint32_t uid;

    float width,height;
    int layer;

    bool animated;

    bool playanimation;
    bool loopanimation;
    float animationdelay;
    int animationindex;
};