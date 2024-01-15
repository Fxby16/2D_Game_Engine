#pragma once

#include <entity.hpp>

class SceneManager;

class Scene{
public:
    Scene(){}
    Scene(const std::string &name);

    std::string &GetName();
    /**
     * Set the scaling factor for the scene.
     * Default value is 0.5 (1 meter = 0.5 units)
    */
    void SetScalingFactor(float scaling_factor);
    /**
     * Set the gravity for the scene (m/s^2)
     * Should be called after OnPhysicsStart()
     * Default value is (0,-0.3)
     * Adjust the gravity according to the scaling factor
    */
    void SetGravity(float x,float y);

    [[nodiscard]] uint32_t AddEntity();
    Entity* GetEntity(uint32_t uid);
    void RemoveEntity(uint32_t uid);

    void MoveEntity(uint32_t uid,float x_offset,float y_offset);
    void SetEntityPosition(uint32_t uid,float x,float y);

    /* Texture Component
    * const char *path,int mag_filter,int min_filter,float width,float height,float layer
    * std::shared_ptr<Texture>texture,float width,float height,float layer 
    */
    /* Animated Texture Component
    * const char *path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter,float width,float height,float layer
    * std::shared_ptr<AnimatedTexture>animated_texture,float width,float height,float layer
    */
    /* Light Component
    * float x_offset,float y_offset,float radius,float blur,Vec3 color,LightType type
    */
    /* Rigidbody Component
    * RigidbodyComponent::BodyType body_type,bool fixed_rotation
    */
    /* Box Collider Component
    * float x_offset,float y_offset,float width,float height,float density,float friction,float restitution,float restitution_threshold
    */
    /* Circle Collider Component
    * float x_offset,float y_offset,float radius,float density,float friction,float restitution,float restitution_threshold
    */
    /* Native Script Component
    * no args
    */
    template<typename T,typename ...Args>
    void AddComponent(uint32_t uid,Args...args){
        PROFILE_FUNCTION();
        
        T temp(std::forward<Args>(args)...,uid);
        
        if constexpr(std::is_same<T,RigidbodyComponent>::value){
            Entity *entity=GetEntity(uid);
            b2BodyDef body_def;
            body_def.type=RigidbodyTypeToBox2DBody(temp.m_BodyType);
            body_def.position.Set(entity->m_X*m_ScalingFactor,entity->m_Y*m_ScalingFactor);
            body_def.angle=0.0f;

            b2Body *body=m_PhysicsWorld->CreateBody(&body_def);
            body->SetFixedRotation(temp.m_FixedRotation);
            temp.m_RuntimeBody=body;
        }else if constexpr(std::is_same<T,BoxColliderComponent>::value){
            RigidbodyComponent *rigidbody=GetComponent<RigidbodyComponent>(uid);
            if(rigidbody==nullptr){
                printf("Entity %d does not have a rigidbody\n",uid);
                return;
            }

            b2PolygonShape box_shape;
            box_shape.SetAsBox(temp.m_Width/2.0f*m_ScalingFactor,temp.m_Height/2.0f*m_ScalingFactor,b2Vec2(temp.m_XOffset*m_ScalingFactor,temp.m_YOffset*m_ScalingFactor),0.0f);

            b2FixtureDef fixture_def;
            fixture_def.shape=&box_shape;
            fixture_def.density=temp.m_Density;
            fixture_def.friction=temp.m_Friction;
            fixture_def.restitution=temp.m_Restitution;
            fixture_def.restitutionThreshold=temp.m_RestitutionThreshold;
            rigidbody->m_RuntimeBody->CreateFixture(&fixture_def);
        }else if constexpr(std::is_same<T,CircleColliderComponent>::value){
            RigidbodyComponent *rigidbody=GetComponent<RigidbodyComponent>(uid);
            if(rigidbody==nullptr){
                printf("Entity %d does not have a rigidbody\n",uid);
                return;
            }

            b2CircleShape circle_shape;
            circle_shape.m_p.Set((temp.m_XOffset)*m_ScalingFactor,(temp.m_YOffset)*m_ScalingFactor);
            circle_shape.m_radius=(temp.m_Radius)*m_ScalingFactor;

            b2FixtureDef fixture_def;
            fixture_def.shape=&circle_shape;
            fixture_def.density=temp.m_Density;
            fixture_def.friction=temp.m_Friction;
            fixture_def.restitution=temp.m_Restitution;
            fixture_def.restitutionThreshold=temp.m_RestitutionThreshold;
            rigidbody->m_RuntimeBody->CreateFixture(&fixture_def);
        }

        AddComponentToContainer<T>(temp,uid);
    }

    template<typename T>
    void RemoveComponent(uint32_t uid);

    template<typename T>
    T* GetComponent(uint32_t uid);

    void OnPhysicsStart();
    void OnPhysicsUpdate(double frame_time);
    void OnPhysicsStop();

    void Update(double frame_time);
    void Render();

    void DebugDraw();
private:

    template<typename T>
    void AddComponentToContainer(T &component,uint32_t uid);

    friend class SceneManager;

    b2World *m_PhysicsWorld=nullptr;
    float m_ScalingFactor=0.5f;

    std::vector<Entity> m_Entities;
    ComponentManager<TextureComponent> m_TextureComponents;
    ComponentManager<AnimatedTextureComponent> m_AnimatedTextureComponents;
    ComponentManager<RigidbodyComponent> m_RigidbodyComponents;
    ComponentManager<BoxColliderComponent> m_BoxColliderComponents;
    ComponentManager<CircleColliderComponent> m_CircleColliderComponents;
    ComponentManager<LightComponent> m_LightComponents;
    ComponentManager<NativeScriptComponent> m_NativeScriptComponents;

    std::string m_Name;
};

class SceneManager{
public:
    SceneManager()=default;
    ~SceneManager();

    void AddScene(const std::string &name);
    void RemoveScene(const std::string &name);
    void SetCurrentScene(const std::string &name);
    Scene* GetCurrentScene();
    inline std::string& GetCurrentSceneName(){ return m_CurrentSceneName; }
private:
    std::string m_CurrentSceneName;

    std::vector<Scene*> m_Scenes;
};