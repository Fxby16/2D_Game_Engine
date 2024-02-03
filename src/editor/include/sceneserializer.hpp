#pragma once

#include <scene.hpp>

class SceneSerializer{
public:
    SceneSerializer()=default;

    void SetScene(Scene *scene);

    void Serialize(const std::string &path);
    bool Deserialize(const std::string &path);

private:
    Scene *m_Scene=nullptr;
};

struct TextureData{
    TextureData(uint32_t uid,float width,float height,float layer):
        uid(uid),width(width),height(height),layer(layer){}

    uint32_t uid;

    float width,height;
    float layer;
};