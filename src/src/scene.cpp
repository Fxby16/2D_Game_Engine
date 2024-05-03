#include <pch.hpp>
#include <scene.hpp>
#include <renderer.hpp>

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

Scene::Scene(const std::string &name): m_Name(name){
    m_ScalingFactor=0.5f;
    OnPhysicsStart();
}

std::string &Scene::GetName(){
    return m_Name;
}

void Scene::SetName(const std::string &name){
    m_Name=name;
}

void Scene::SetScalingFactor(float scaling_factor){
    m_ScalingFactor=scaling_factor;
}

void Scene::SetGravity(float x,float y){
    m_Gravity={x,y};
    m_PhysicsWorld->SetGravity(b2Vec2(x,y));
}

void Scene::SetGravity(Vec2 gravity){
    m_Gravity=gravity;
    m_PhysicsWorld->SetGravity(b2Vec2(gravity.x,gravity.y));
}

uint32_t Scene::AddEntity(uint32_t parent){
    m_Entities.push_back(Entity(parent));
    if(parent!=std::numeric_limits<uint32_t>::max()){
        m_Hierarchy[parent].insert(m_Entities.back().m_UID);
    }
    m_Hierarchy.insert({m_Entities.back().m_UID,{}});
    return m_Entities.back().m_UID;
}

void Scene::AddEntity(uint32_t uid,uint32_t parent,uint32_t group){
    if(parent!=std::numeric_limits<uint32_t>::max()){
        m_Hierarchy[parent].insert(uid);
    }
    m_Hierarchy.insert({uid,{}});
    m_Entities.push_back(Entity(uid,parent,group));
}

uint32_t Scene::DuplicateEntity(uint32_t uid,uint32_t parent){
    uint32_t new_entity=AddEntity();
    Entity *to_duplicate=GetEntity(uid);
    Entity *duplicated=GetEntity(new_entity);
    *duplicated=*to_duplicate;
    duplicated->m_UID=new_entity;

    if(parent!=std::numeric_limits<uint32_t>::max()){
        duplicated->m_Parent=parent;
    }

    m_Hierarchy.insert({new_entity,{}});
    if(duplicated->m_Parent!=std::numeric_limits<uint32_t>::max()){
        m_Hierarchy[duplicated->m_Parent].insert(new_entity);
    }

    TagComponent *tag=GetComponent<TagComponent>(uid);
    TextureComponent *tc=GetComponent<TextureComponent>(uid);
    AnimatedTextureComponent *atc=GetComponent<AnimatedTextureComponent>(uid);
    RigidbodyComponent *rbc=GetComponent<RigidbodyComponent>(uid);
    BoxColliderComponent *bcc=GetComponent<BoxColliderComponent>(uid);
    CircleColliderComponent *ccc=GetComponent<CircleColliderComponent>(uid);
    LightComponent *lc=GetComponent<LightComponent>(uid);
    NativeScriptComponent *nsc=GetComponent<NativeScriptComponent>(uid);
    TextComponent *ttc=GetComponent<TextComponent>(uid);

    if(tag!=nullptr){
        AddComponent<TagComponent>(new_entity,tag->m_Tag);
    }
    if(tc!=nullptr){
        AddComponent<TextureComponent>(new_entity,tc->m_Texture->m_LoadedFilePath,tc->m_Texture->m_LoadedMagFilter,tc->m_Texture->m_LoadedMinFilter,tc->m_Width,tc->m_Height,tc->m_Layer,tc->m_Visible,tc->m_FlipX,tc->m_FlipY);
    }
    if(atc!=nullptr){
        AddComponent<AnimatedTextureComponent>(new_entity,atc->m_AnimatedTexture->m_LoadedFilePath,atc->m_AnimatedTexture->m_TileWidth,atc->m_AnimatedTexture->m_TileHeight,atc->m_AnimatedTexture->m_LoadedMagFilter,atc->m_AnimatedTexture->m_LoadedMinFilter,atc->m_Width,atc->m_Height,atc->m_Layer,atc->m_PlayAnimation,atc->m_LoopAnimation,atc->m_AnimationDelay,atc->m_AnimationRow,atc->m_AnimationIndex,atc->m_Visible,atc->m_FlipX,atc->m_FlipY);
    }
    if(rbc!=nullptr){
        AddComponent<RigidbodyComponent>(new_entity,rbc->m_BodyType,rbc->m_FixedRotation);
    }
    if(bcc!=nullptr){
        AddComponent<BoxColliderComponent>(new_entity,bcc->m_XOffset,bcc->m_YOffset,bcc->m_Width,bcc->m_Height,bcc->m_Density,bcc->m_Friction,bcc->m_Restitution,bcc->m_RestitutionThreshold,bcc->m_CategoryBits,bcc->m_MaskBits,bcc->m_IsSensor);
    }
    if(ccc!=nullptr){
        AddComponent<CircleColliderComponent>(new_entity,ccc->m_XOffset,ccc->m_YOffset,ccc->m_Radius,ccc->m_Density,ccc->m_Friction,ccc->m_Restitution,ccc->m_RestitutionThreshold,ccc->m_CategoryBits,ccc->m_MaskBits,ccc->m_IsSensor);
    }
    if(lc!=nullptr){
        AddComponent<LightComponent>(new_entity,lc->m_XOffset,lc->m_YOffset,lc->m_Radius,lc->m_Blur,lc->m_Color,lc->m_Type);
    }
    if(nsc!=nullptr){
        AddComponent<NativeScriptComponent>(new_entity);
    }
    if(ttc!=nullptr){
        AddComponent<TextComponent>(new_entity,ttc->m_TextRenderer->m_LoadedFontPath,ttc->m_TextRenderer->m_LoadedGlyphSize,ttc->m_TextRenderer->m_Fixed,ttc->m_Text,ttc->m_Offset,ttc->m_Color,ttc->m_Scale,ttc->m_Layer,ttc->m_IgnoreLighting);
    }

    for(auto &entity:m_Hierarchy[uid]){
        DuplicateEntity(entity,new_entity);
    }

    return new_entity;
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

    std::stack<uint32_t> entities;
    std::vector<uint32_t> to_remove;
    
    entities.push(uid);
    to_remove.push_back(uid);

    while(!entities.empty()){

        uint32_t current_uid=entities.top();
        entities.pop();
    
        m_TagComponents.RemoveComponent(current_uid);
        m_TextureComponents.RemoveComponent(current_uid);
        m_AnimatedTextureComponents.RemoveComponent(current_uid);
        m_LightComponents.RemoveComponent(current_uid);
        m_BoxColliderComponents.RemoveComponent(current_uid);
        m_CircleColliderComponents.RemoveComponent(current_uid);
        m_RigidbodyComponents.RemoveComponent(current_uid);
        m_NativeScriptComponents.RemoveComponent(current_uid);
        m_TextComponents.RemoveComponent(current_uid);

        for(auto derived:m_Hierarchy[current_uid]){ //add to the stack all the derived entities that must be removed
            entities.push(derived);
            to_remove.push_back(derived);
        }

        m_Hierarchy.erase(current_uid);
        int idx=BinarySearchi(m_Entities,current_uid);
        if(idx!=-1){
            m_Entities.erase(m_Entities.begin()+idx);
        }
    }

    for(auto &entity:m_Hierarchy){
        for(auto &derived:to_remove){
            entity.second.erase(derived);
        }
    }
}

void Scene::SetEntityPosition(uint32_t uid,float x,float y){
    Entity *entity=GetEntity(uid);
    if(entity==nullptr)
        return;

    while(entity->m_Parent!=std::numeric_limits<uint32_t>::max()){
        entity=GetEntity(entity->m_Parent);
    }

    entity->m_X=x;
    entity->m_PreviousX=x;
    entity->m_Y=y;
    entity->m_PreviousY=y;

    if(m_PhysicsWorld!=nullptr){
        RigidbodyComponent *rigidbody=GetComponent<RigidbodyComponent>(uid);
        if(rigidbody==nullptr){ return; }
        rigidbody->m_RuntimeBody->SetTransform(b2Vec2(x*m_ScalingFactor,y*m_ScalingFactor),0.0f);
    }
}

template<>
void Scene::AddComponentToContainer<TagComponent>(TagComponent &component,uint32_t uid){
    PROFILE_FUNCTION();
    m_TagComponents.AddComponent(component,uid);
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
void Scene::AddComponentToContainer<TextComponent>(TextComponent &component,uint32_t uid){
    PROFILE_FUNCTION();
    m_TextComponents.AddComponent(component,uid);
}

template<>
void Scene::RemoveComponent<TagComponent>(uint32_t uid){
    PROFILE_FUNCTION();
    
    m_TagComponents.RemoveComponent(uid);
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

template<>
void Scene::RemoveComponent<TextComponent>(uint32_t uid){
    PROFILE_FUNCTION();
    
    m_TextComponents.RemoveComponent(uid);
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
    m_TextComponents.Render(this);
    m_LightComponents.Render(this);
}

void Scene::MoveEntity(uint32_t uid,float x_offset,float y_offset){
    PROFILE_FUNCTION();
    
    RigidbodyComponent *rigidbody=GetComponent<RigidbodyComponent>(uid);
    if(rigidbody==nullptr){
        printf("Entity %d does not have a rigidbody\n",uid);
        return;
    }
    Entity *entity=GetEntity(uid);
    if(entity==nullptr){
        return;
    }

    while(entity->m_Parent!=std::numeric_limits<uint32_t>::max()){
        entity=GetEntity(entity->m_Parent);
    }

    const b2Vec2 &vel=rigidbody->m_RuntimeBody->GetLinearVelocity();
    float mass=rigidbody->m_RuntimeBody->GetMass();

    rigidbody->m_RuntimeBody->ApplyLinearImpulse(b2Vec2(mass*((x_offset*m_ScalingFactor)-vel.x),((y_offset!=0)?(mass*((y_offset*m_ScalingFactor)-vel.y)):0)),rigidbody->m_RuntimeBody->GetWorldCenter(),true);
}

void Scene::OnPhysicsStart(){
    m_PhysicsWorld=new b2World(b2Vec2(m_Gravity.x,m_Gravity.y));
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

std::vector<Entity> &Scene::GetEntities(){
    return m_Entities;
}

std::pair<float,float> Scene::GetEntityXY(uint32_t uid){
    return {GetEntityX(uid),GetEntityY(uid)};
}

std::pair<float,float> Scene::GetEntityPreviousXY(uint32_t uid){
    return {GetEntityPreviousX(uid),GetEntityPreviousY(uid)};
}

float Scene::GetEntityX(uint32_t uid){
    Entity *entity=GetEntity(uid);
    if(entity==nullptr)
        return 0.0f;
    
    while(entity->m_Parent!=std::numeric_limits<uint32_t>::max()){
        entity=GetEntity(entity->m_Parent);
    }

    return entity->m_X;
}

float Scene::GetEntityY(uint32_t uid){
    Entity *entity=GetEntity(uid);
    if(entity==nullptr)
        return 0.0f;
    
    while(entity->m_Parent!=std::numeric_limits<uint32_t>::max()){
        entity=GetEntity(entity->m_Parent);
    }

    return entity->m_Y;
}

float Scene::GetEntityPreviousX(uint32_t uid){
    Entity *entity=GetEntity(uid);
    if(entity==nullptr)
        return 0.0f;
    
    while(entity->m_Parent!=std::numeric_limits<uint32_t>::max()){
        entity=GetEntity(entity->m_Parent);
    }

    return entity->m_PreviousX;
}

float Scene::GetEntityPreviousY(uint32_t uid){
    Entity *entity=GetEntity(uid);
    if(entity==nullptr)
        return 0.0f;
    
    while(entity->m_Parent!=std::numeric_limits<uint32_t>::max()){
        entity=GetEntity(entity->m_Parent);
    }

    return entity->m_PreviousY;
}

template<>
std::vector<TextureComponent> &Scene::GetComponents<TextureComponent>(){
    return m_TextureComponents.m_Components;
}

template<>
std::vector<AnimatedTextureComponent> &Scene::GetComponents<AnimatedTextureComponent>(){
    return m_AnimatedTextureComponents.m_Components;
}

template<>
std::vector<LightComponent> &Scene::GetComponents<LightComponent>(){
    return m_LightComponents.m_Components;
}

template<>
std::vector<RigidbodyComponent> &Scene::GetComponents<RigidbodyComponent>(){
    return m_RigidbodyComponents.m_Components;
}

template<>
std::vector<BoxColliderComponent> &Scene::GetComponents<BoxColliderComponent>(){
    return m_BoxColliderComponents.m_Components;
}

template<>
std::vector<CircleColliderComponent> &Scene::GetComponents<CircleColliderComponent>(){
    return m_CircleColliderComponents.m_Components;
}

template<>
std::vector<NativeScriptComponent> &Scene::GetComponents<NativeScriptComponent>(){
    return m_NativeScriptComponents.m_Components;
}

template<>
std::vector<TextComponent> &Scene::GetComponents<TextComponent>(){
    return m_TextComponents.m_Components;
}

void Scene::DebugDraw(){
    float current_line_width=RENDERER->GetLineWidth();
    RENDERER->SetLineWidth(0.03f);

    for(auto &entity:m_Entities){
        BoxColliderComponent *box_collider=GetComponent<BoxColliderComponent>(entity.m_UID);
        if(box_collider!=nullptr){
            float m_X=GetEntityX(entity.m_UID);
            float m_Y=GetEntityY(entity.m_UID);
            float m_PreviousX=GetEntityPreviousX(entity.m_UID);
            float m_PreviousY=GetEntityPreviousY(entity.m_UID);
            RENDERER->DrawLine({Interpolate(m_X,m_PreviousX),Interpolate(m_Y,m_PreviousY)},{Interpolate(m_X,m_PreviousX)+box_collider->m_Width,Interpolate(m_Y,m_PreviousY)},{0,1,0.75f,1},5);
            RENDERER->DrawLine({Interpolate(m_X,m_PreviousX)+box_collider->m_Width,Interpolate(m_Y,m_PreviousY)},{Interpolate(m_X,m_PreviousX)+box_collider->m_Width,Interpolate(m_Y,m_PreviousY)+box_collider->m_Height},{0,1,0.75f,1},5);
            RENDERER->DrawLine({Interpolate(m_X,m_PreviousX)+box_collider->m_Width,Interpolate(m_Y,m_PreviousY)+box_collider->m_Height},{Interpolate(m_X,m_PreviousX),Interpolate(m_Y,m_PreviousY)+box_collider->m_Height},{0,1,0.75f,1},5);
            RENDERER->DrawLine({Interpolate(m_X,m_PreviousX),Interpolate(m_Y,m_PreviousY)+box_collider->m_Height},{Interpolate(m_X,m_PreviousX),Interpolate(m_Y,m_PreviousY)},{0,1,0.75f,1},5);
        }
        CircleColliderComponent *circle_collider=GetComponent<CircleColliderComponent>(entity.m_UID);
        if(circle_collider!=nullptr){
            float current_point_size=RENDERER->GetPointSize();
            float m_X=GetEntityX(entity.m_UID);
            float m_Y=GetEntityY(entity.m_UID);
            float m_PreviousX=GetEntityPreviousX(entity.m_UID);
            float m_PreviousY=GetEntityPreviousY(entity.m_UID);
            RENDERER->SetPointSize(circle_collider->m_Radius*2);
            RENDERER->DrawCircle({Interpolate(m_X,m_PreviousX)+circle_collider->m_XOffset,Interpolate(m_Y,m_PreviousY)+circle_collider->m_YOffset},{0,1,0.75f,0.5f},0.2f,5);
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