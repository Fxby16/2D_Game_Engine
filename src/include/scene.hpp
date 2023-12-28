#pragma once

#include <entity.hpp>

class SceneManager;

class Scene{
public:
    Scene(){}
    Scene(const std::string &name);

    uint64_t AddEntity();
    Entity* GetEntity(uint64_t uid);
    void RemoveEntity(uint64_t uid);

    void MoveEntity(uint64_t uid,float x_offset,float y_offset);
    void SetEntityPosition(uint64_t uid,float x,float y);

    template<typename T,typename...Args>
    void AddComponent(uint64_t uid,Args...args);

    template<typename T>
    void RemoveComponent(uint64_t uid);

    template<typename T>
    T* GetComponent(uint64_t uid);

    void Update(double frame_time);
    void Render();
private:
    friend class SceneManager;

    std::vector<Entity> m_Entities;
    ComponentManager<TextureComponent> m_TextureComponents;
    ComponentManager<AnimatedTextureComponent> m_AnimatedTextureComponents;
    ComponentManager<ColliderComponent> m_ColliderComponents;
    ComponentManager<LightComponent> m_LightComponents;

    std::string m_Name;
};

class SceneManager{
public:
    SceneManager(){}

    void AddScene(const std::string &name);
    void RemoveScene(const std::string &name);
    void SetCurrentScene(const std::string &name);
    Scene* GetCurrentScene();
    inline std::string& GetCurrentSceneName(){ return m_CurrentSceneName; }
private:
    std::string m_CurrentSceneName;

    std::vector<Scene> m_Scenes;
};