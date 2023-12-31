#pragma once

#include <entity.hpp>

class SceneManager;
class b2World;

class Scene{
public:
    Scene(){}
    Scene(const std::string &name);

    std::string &GetName();
    /**
     * Set the scaling factor for the scene.
     * Example:
     * If you want 100px to be 1 meter in the physics world,then set the scaling factor to 0.01f.
    */
    void SetScalingFactor(float scaling_factor);
    /**
     * Set the gravity for the scene (m/s^2)
     * Should be called after OnPhysicsStart()
     * Default value is (0,-9.8)
    */
    void SetGravity(float x,float y);

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

    void OnPhysicsStart();
    void OnPhysicsUpdate(double frame_time);
    void OnPhysicsStop();

    void Update(double frame_time);
    void Render();
private:
    friend class SceneManager;

    b2World *m_PhysicsWorld=nullptr;
    float m_ScalingFactor=0.01f;

    std::vector<Entity> m_Entities;
    ComponentManager<TextureComponent> m_TextureComponents;
    ComponentManager<AnimatedTextureComponent> m_AnimatedTextureComponents;
    ComponentManager<RigidbodyComponent> m_RigidbodyComponents;
    ComponentManager<BoxColliderComponent> m_BoxColliderComponents;
    ComponentManager<CircleColliderComponent> m_CircleColliderComponents;
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