#pragma once

#include <scene.hpp>

class SceneSerializer{
public:
    SceneSerializer()=default;

    void SetScene(Scene *scene);

    void Serialize(const std::string &path,std::vector<std::pair<std::string,uint32_t>>&script_components);
    void SerializeEncrypted(const std::string &path,std::vector<std::pair<std::string,uint32_t>>&script_components);
    void SerializeEntity(YAML::Emitter &out,Entity &entity,Scene *scene,std::vector<std::pair<std::string,uint32_t>>&script_components);
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