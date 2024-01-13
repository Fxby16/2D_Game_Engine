#include <scene.hpp>
#include <box2d/box2d.h>
#include <renderer.hpp>
#include <Instrumentor.h>

int BinarySearchi(std::vector<Entity> &v,uint32_t uid){
    int l=0,r=v.size()-1;
    while(l<=r){
        int m=(l+r)/2;
        if(v[m].m_UID==uid)
            return m;
        else if(v[m].m_UID<uid)
            l=m+1;
        else
            r=m-1;
    }
    return -1;
}

Scene::Scene(const std::string &name): m_Name(name){}

std::string &Scene::GetName(){
    return m_Name;
}

void Scene::SetScalingFactor(float scaling_factor){
    m_ScalingFactor=scaling_factor;
}

void Scene::SetGravity(float x,float y){
    m_PhysicsWorld->SetGravity(b2Vec2(x,y));
}

uint32_t Scene::AddEntity(){
    m_Entities.push_back(Entity());
    return m_Entities.back().m_UID;
}

Entity* Scene::GetEntity(uint32_t uid){
    PROFILE_FUNCTION();

    int idx=BinarySearchi(m_Entities,uid);
    if(idx==-1)
        return nullptr;
    return &m_Entities[idx];
}

void Scene::RemoveEntity(uint32_t uid){
    PROFILE_FUNCTION();

    m_TextureComponents.RemoveComponent(uid);
    m_AnimatedTextureComponents.RemoveComponent(uid);
    m_LightComponents.RemoveComponent(uid);

    int idx=BinarySearchi(m_Entities,uid);
    if(idx==-1)
        return;
    m_Entities.erase(m_Entities.begin()+idx);
}

template<>
TextureComponent* Scene::GetComponent<TextureComponent>(uint32_t uid){
    PROFILE_FUNCTION();
    
    return m_TextureComponents.GetComponent(uid);
}

template<>
AnimatedTextureComponent* Scene::GetComponent<AnimatedTextureComponent>(uint32_t uid){
    PROFILE_FUNCTION();
    
    return m_AnimatedTextureComponents.GetComponent(uid);
}

template<>
LightComponent* Scene::GetComponent<LightComponent>(uint32_t uid){
    PROFILE_FUNCTION();
    
    return m_LightComponents.GetComponent(uid);
}

template<>
RigidbodyComponent* Scene::GetComponent<RigidbodyComponent>(uint32_t uid){
    PROFILE_FUNCTION();
    
    return m_RigidbodyComponents.GetComponent(uid);
}

template<>
BoxColliderComponent* Scene::GetComponent<BoxColliderComponent>(uint32_t uid){
    PROFILE_FUNCTION();

    return m_BoxColliderComponents.GetComponent(uid);
}

template<>
CircleColliderComponent* Scene::GetComponent<CircleColliderComponent>(uint32_t uid){
    PROFILE_FUNCTION();

    return m_CircleColliderComponents.GetComponent(uid);
}

template<>
NativeScriptComponent* Scene::GetComponent<NativeScriptComponent>(uint32_t uid){
    PROFILE_FUNCTION();

    return m_NativeScriptComponents.GetComponent(uid);
}

void Scene::SetEntityPosition(uint32_t uid,float x,float y){
    Entity *entity=GetEntity(uid);
    if(entity==nullptr)
        return;
    entity->m_X=x;
    entity->m_Y=y;

    if(m_PhysicsWorld!=nullptr){
        RigidbodyComponent *rigidbody=GetComponent<RigidbodyComponent>(uid);
        if(rigidbody==nullptr){ return; }
        rigidbody->m_RuntimeBody->SetTransform(b2Vec2(x*m_ScalingFactor,y*m_ScalingFactor),0.0f);
    }
}

template<>
void Scene::AddComponentToContainer<TextureComponent>(TextureComponent &component,uint32_t uid){
    PROFILE_FUNCTION();
    m_TextureComponents.AddComponent(component,uid);
}   

template<>
void Scene::AddComponentToContainer<AnimatedTextureComponent>(AnimatedTextureComponent &component,uint32_t uid){
    PROFILE_FUNCTION();
    m_AnimatedTextureComponents.AddComponent(component,uid);
}

template<>
void Scene::AddComponentToContainer<LightComponent>(LightComponent &component,uint32_t uid){
    PROFILE_FUNCTION();
    m_LightComponents.AddComponent(component,uid);
}

template<>
void Scene::AddComponentToContainer<RigidbodyComponent>(RigidbodyComponent &component,uint32_t uid){
    PROFILE_FUNCTION();
    m_RigidbodyComponents.AddComponent(component,uid);
}

template<>
void Scene::AddComponentToContainer<BoxColliderComponent>(BoxColliderComponent &component,uint32_t uid){
    PROFILE_FUNCTION();
    m_BoxColliderComponents.AddComponent(component,uid);
}

template<>
void Scene::AddComponentToContainer<CircleColliderComponent>(CircleColliderComponent &component,uint32_t uid){
    PROFILE_FUNCTION();
    m_CircleColliderComponents.AddComponent(component,uid);
}

template<>
void Scene::AddComponentToContainer<NativeScriptComponent>(NativeScriptComponent &component,uint32_t uid){
    PROFILE_FUNCTION();
    m_NativeScriptComponents.AddComponent(component,uid);
}

template<>
void Scene::RemoveComponent<TextureComponent>(uint32_t uid){
    PROFILE_FUNCTION();
    
    m_TextureComponents.RemoveComponent(uid);
}

template<>
void Scene::RemoveComponent<AnimatedTextureComponent>(uint32_t uid){
    PROFILE_FUNCTION();
    
    m_AnimatedTextureComponents.RemoveComponent(uid);
}

template<>
void Scene::RemoveComponent<LightComponent>(uint32_t uid){
    PROFILE_FUNCTION();
    
    m_LightComponents.RemoveComponent(uid);
}

template<>
void Scene::RemoveComponent<RigidbodyComponent>(uint32_t uid){
    PROFILE_FUNCTION();
    
    m_RigidbodyComponents.RemoveComponent(uid);
}

template<> 
void Scene::RemoveComponent<BoxColliderComponent>(uint32_t uid){
    PROFILE_FUNCTION();
    
    m_BoxColliderComponents.RemoveComponent(uid);
}

template<>
void Scene::RemoveComponent<CircleColliderComponent>(uint32_t uid){
    PROFILE_FUNCTION();
    
    m_CircleColliderComponents.RemoveComponent(uid);
}

template<>
void Scene::RemoveComponent<NativeScriptComponent>(uint32_t uid){
    PROFILE_FUNCTION();
    
    m_NativeScriptComponents.RemoveComponent(uid);
}

void Scene::Update(double frame_time){
    PROFILE_FUNCTION();

    m_NativeScriptComponents.Update(this,frame_time);
    OnPhysicsUpdate(frame_time);
}

void Scene::Render(){
    PROFILE_FUNCTION();

    m_TextureComponents.Render(this);
    m_AnimatedTextureComponents.Render(this);
    m_LightComponents.Render(this);
}

SceneManager::~SceneManager(){
    for(auto scene:m_Scenes)
        delete scene;
}

void SceneManager::AddScene(const std::string &name){
    m_Scenes.push_back(new Scene(name));
}

void SceneManager::RemoveScene(const std::string &name){
    if(name==m_CurrentSceneName)
        m_CurrentSceneName="";
    for(int i=0;i<m_Scenes.size();i++){
        if(m_Scenes[i]->m_Name==name){
            m_Scenes.erase(m_Scenes.begin()+i);
            break;
        }
    }
}

void SceneManager::SetCurrentScene(const std::string &name){
    m_CurrentSceneName=name;
}

Scene* SceneManager::GetCurrentScene(){
    if(m_CurrentSceneName.empty()){
        printf("You must set a current scene before calling GetCurrentScene()\n");
        return nullptr;
    }
    for(auto scene:m_Scenes){
        if(scene->m_Name==m_CurrentSceneName)
            return scene;
    }
    return nullptr;
}

void Scene::MoveEntity(uint32_t uid,float x_offset,float y_offset){
    PROFILE_FUNCTION();
    
    RigidbodyComponent *rigidbody=GetComponent<RigidbodyComponent>(uid);
    if(rigidbody==nullptr){
        printf("Entity %d does not have a rigidbody\n",uid);
        return;
    }
    Entity *entity=GetEntity(uid);

    rigidbody->m_RuntimeBody->SetLinearVelocity(b2Vec2(x_offset*m_ScalingFactor,y_offset*m_ScalingFactor));
}

void Scene::OnPhysicsStart(){
    m_PhysicsWorld=new b2World(b2Vec2(0.0f,-0.3f));
}

void Scene::OnPhysicsUpdate(double frame_time){
    PROFILE_FUNCTION();
    
    const int velocity_iterations=6;
    const int position_iterations=2;
    m_PhysicsWorld->Step(frame_time,velocity_iterations,position_iterations);

    std::vector<RigidbodyComponent> &rigidbodies=m_RigidbodyComponents.m_Components;
    Entity *entity;
    b2Body *body;
    for(int i=0;i<rigidbodies.size();i++){
        entity=GetEntity(rigidbodies[i].m_UID);
        body=rigidbodies[i].m_RuntimeBody;
        const auto &position=body->GetPosition();
        entity->m_PreviousX=entity->m_X;
        entity->m_PreviousY=entity->m_Y;
        entity->m_X=position.x/m_ScalingFactor;
        entity->m_Y=position.y/m_ScalingFactor;
    }
}

void Scene::OnPhysicsStop(){
    delete m_PhysicsWorld;
    m_PhysicsWorld=nullptr;
}

void Scene::DebugDraw(){
    float current_line_width=RENDERER->GetLineWidth();
    RENDERER->SetLineWidth(0.03f);

    for(auto &entity:m_Entities){
        BoxColliderComponent *box_collider=GetComponent<BoxColliderComponent>(entity.m_UID);
        if(box_collider!=nullptr){
            RENDERER->DrawLine({Interpolate(entity.m_X,entity.m_PreviousX),Interpolate(entity.m_Y,entity.m_PreviousY)},{Interpolate(entity.m_X,entity.m_PreviousX)+box_collider->m_Width,Interpolate(entity.m_Y,entity.m_PreviousY)},{0,1,0.75f,1},5);
            RENDERER->DrawLine({Interpolate(entity.m_X,entity.m_PreviousX)+box_collider->m_Width,Interpolate(entity.m_Y,entity.m_PreviousY)},{Interpolate(entity.m_X,entity.m_PreviousX)+box_collider->m_Width,Interpolate(entity.m_Y,entity.m_PreviousY)+box_collider->m_Height},{0,1,0.75f,1},5);
            RENDERER->DrawLine({Interpolate(entity.m_X,entity.m_PreviousX)+box_collider->m_Width,Interpolate(entity.m_Y,entity.m_PreviousY)+box_collider->m_Height},{Interpolate(entity.m_X,entity.m_PreviousX),Interpolate(entity.m_Y,entity.m_PreviousY)+box_collider->m_Height},{0,1,0.75f,1},5);
            RENDERER->DrawLine({Interpolate(entity.m_X,entity.m_PreviousX),Interpolate(entity.m_Y,entity.m_PreviousY)+box_collider->m_Height},{Interpolate(entity.m_X,entity.m_PreviousX),Interpolate(entity.m_Y,entity.m_PreviousY)},{0,1,0.75f,1},5);
        }
        CircleColliderComponent *circle_collider=GetComponent<CircleColliderComponent>(entity.m_UID);
        if(circle_collider!=nullptr){
            float current_point_size=RENDERER->GetPointSize();
            RENDERER->SetPointSize(circle_collider->m_Radius*2);
            RENDERER->DrawPoint({Interpolate(entity.m_X,entity.m_PreviousX)+circle_collider->m_XOffset,Interpolate(entity.m_Y,entity.m_PreviousY)+circle_collider->m_YOffset},{0,1,0.75f,0.5f},5);
            RENDERER->Render();
            RENDERER->SetPointSize(current_point_size);
        }
    }

    auto segments=RENDERER->GetSegments();
    for(auto &segment:segments){
        RENDERER->DrawLine({segment.first.x,segment.first.y},{segment.second.x,segment.second.y},{0,1,0.75f,1.0f},5);
    }
    RENDERER->Render();
    RENDERER->SetLineWidth(current_line_width);
}