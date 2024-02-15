#pragma once

#include <scene.hpp>

class SceneSerializer{
public:
    SceneSerializer()=default;

    void SetScene(Scene *scene);

    void Serialize(const std::string &path);
    void SerializeEncrypted(const std::string &path);
    bool Deserialize(const std::string &path);
    bool DeserializeEncrypted(const std::string &path);
    bool DeserializeNode(const YAML::Node &data);

private:
    Scene *m_Scene=nullptr;
};

struct TextureData{
    TextureData(uint32_t uid,float width,float height,float layer):
        uid(uid),width(width),height(height),layer(layer),animated(false){}
    TextureData(uint32_t uid,float width,float height,float layer,bool playanimation,
        bool loopanimation,float animationdelay,int animationindex): uid(uid),width(width),height(height),layer(layer),
        animated(true),playanimation(playanimation),loopanimation(loopanimation),animationdelay(animationdelay),animationindex(animationindex){}

    uint32_t uid;

    float width,height;
    float layer;

    bool animated;

    bool playanimation;
    bool loopanimation;
    float animationdelay;
    int animationindex;
};