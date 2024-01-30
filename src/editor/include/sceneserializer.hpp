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