#pragma once

#include <entity.hpp>
#include <camera.hpp>

class Scene{
public:
    /**
     * Calls OnPhysicsStart and sets the scene name to ""
    */
    Scene(){
        OnPhysicsStart();
    }
    /**
     * Calls OnPhysicsStart
     * \param name the scene name
    */
    Scene(const std::string &name);

    std::string &GetName();
    /**
     * Set the scaling factor for the scene. 
     * Default value is 0.5 (1 meter = 0.5 units)
    */
    void SetScalingFactor(float scaling_factor);
    /**
     * Set the gravity for the scene (m/s^2). 
     * Should be called after OnPhysicsStart(). 
     * Default value is (0,-0.3). 
     * Adjust the gravity according to the scaling factor. 
    */
    void SetGravity(float x,float y);

    /**
     * Set the gravity for the scene (m/s^2). 
     * Should be called after OnPhysicsStart(). 
     * Default value is (0,-0.3). 
     * Adjust the gravity according to the scaling factor. 
    */
    void SetGravity(Vec2 gravity);

    /**
     * Add an entity to the scene
     * \return the entity uid
    */
    [[nodiscard]] uint32_t AddEntity();
    /**
     * Add an entity to the scene. 
     * Used when deserializing a scene. 
     * \param uid the entity uid
    */
    void AddEntity(uint32_t uid);
    /**
     * Get an entity from the scene
     * \param uid the entity uid
     * \return a pointer to the entity or nullptr if the entity does not exist
    */
    Entity* GetEntity(uint32_t uid);
    /**
     * Remove an entity from the scene
     * \param uid the entity uid
    */
    void RemoveEntity(uint32_t uid);

    /**
     * Move the entity. 
     * The coordinates are expected in engine units (screen width = 10 units)
    */
    void MoveEntity(uint32_t uid,float x_offset,float y_offset);
    void SetEntityPosition(uint32_t uid,float x,float y);

    /**
     * Add a component to an entity
     * \param uid the entity uid
     * \param args the component constructor arguments
    */
    template<typename T,typename ...Args>
    void AddComponent(uint32_t uid,Args...args){
        PROFILE_FUNCTION();
        
        T temp(std::forward<Args>(args)...,uid);

        // auto printArg=[](const auto &arg){ std::cout<<arg<<' '; };
        // (printArg(args),...);

        // std::cout<<std::endl;
        
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

    /**
     * Remove a component from an entity
     * \param uid the entity uid
    */
    template<typename T>
    void RemoveComponent(uint32_t uid);

    /**
     * Get a component from an entity
     * \param uid the entity uid
     * \return a pointer to the component or nullptr if the component does not exist
    */
    template<typename T>
    T* GetComponent(uint32_t uid);

    /**
     * Starts the physics simulation
    */
    void OnPhysicsStart();
    /**
     * Update the physics simulation
     * \param frame_time the frame time
    */
    void OnPhysicsUpdate(double frame_time);
    /**
     * Stops the physics simulation
    */
    void OnPhysicsStop();

    /**
     * Update the scene
     * \param frame_time the frame time
    */
    void Update(double frame_time);
    /**
     * Render the scene
    */
    void Render();

    /**
     * Get the entities vector
     * \return a reference to the entities vector
    */
    std::vector<Entity> &GetEntities();
    /**
     * Get the components vector
     * \return a reference to the components vector
    */
    template<typename T,ComponentType<T> = 0>
    std::vector<T> &GetComponents();

    inline Camera &GetCamera(){ return m_Camera; }

    void DebugDraw();
private:
    friend class SceneSerializer;
    friend class Application;

    /**
     * Add a component to the container relative to its type
     * \param component the component
     * \param uid the entity uid
    */
    template<typename T>
    void AddComponentToContainer(T &component,uint32_t uid);

    b2World *m_PhysicsWorld=nullptr;
    float m_ScalingFactor=0.5f;
    Vec2 m_Gravity;

    Camera m_Camera;

    std::vector<Entity> m_Entities;
    ComponentManager<TagComponent> m_TagComponents;
    ComponentManager<TextureComponent> m_TextureComponents;
    ComponentManager<AnimatedTextureComponent> m_AnimatedTextureComponents;
    ComponentManager<RigidbodyComponent> m_RigidbodyComponents;
    ComponentManager<BoxColliderComponent> m_BoxColliderComponents;
    ComponentManager<CircleColliderComponent> m_CircleColliderComponents;
    ComponentManager<LightComponent> m_LightComponents;
    ComponentManager<NativeScriptComponent> m_NativeScriptComponents;

    std::vector<std::vector<uint32_t>> m_TexturesEntities; ///< entities with the same texture are grouped together
    std::vector<std::vector<uint32_t>> m_AnimatedTexturesEntities; ///< entities with the same animated texture are grouped together

    std::string m_Name;
};