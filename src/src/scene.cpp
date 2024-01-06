#include <scene.hpp>
#include <box2d/box2d.h>
#include <renderer.hpp>

int BinarySearchi(std::vector<Entity> &v,uint64_t uid){
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

uint64_t Scene::AddEntity(){
    m_Entities.push_back(Entity());
    return m_Entities.back().m_UID;
}

Entity* Scene::GetEntity(uint64_t uid){
    int idx=BinarySearchi(m_Entities,uid);
    if(idx==-1)
        return nullptr;
    return &m_Entities[idx];
}

void Scene::RemoveEntity(uint64_t uid){
    m_TextureComponents.RemoveComponent(uid);
    m_AnimatedTextureComponents.RemoveComponent(uid);
    m_LightComponents.RemoveComponent(uid);

    int idx=BinarySearchi(m_Entities,uid);
    if(idx==-1)
        return;
    m_Entities.erase(m_Entities.begin()+idx);
}

void Scene::SetEntityPosition(uint64_t uid,float x,float y){
    Entity *entity=GetEntity(uid);
    if(entity==nullptr)
        return;
    entity->m_X=x;
    entity->m_Y=y;
}

template<>
void Scene::AddComponent<TextureComponent,const char*,int,int,float,float,float>(uint64_t uid,const char *path,int mag_filter,int min_filter,float width,float height,float layer){
    TextureComponent temp(path,mag_filter,min_filter,width,height,layer,uid);
    m_TextureComponents.AddComponent(temp,uid);
}

template<>
void Scene::AddComponent<TextureComponent,std::shared_ptr<Texture>,float,float,float>(uint64_t uid,std::shared_ptr<Texture>texture,float width,float height,float layer){
    TextureComponent temp(texture,width,height,layer,uid);
    m_TextureComponents.AddComponent(temp,uid);
}

template<>
void Scene::AddComponent<AnimatedTextureComponent,const char*,unsigned int,unsigned int,int,int,float,float,float>(uint64_t uid,const char *path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter,float width,float height,float layer){
    AnimatedTextureComponent temp(path,tile_width,tile_height,mag_filter,min_filter,width,height,layer,uid);
    m_AnimatedTextureComponents.AddComponent(temp,uid);
}

template<>
void Scene::AddComponent<AnimatedTextureComponent,std::shared_ptr<AnimatedTexture>,float,float,float>(uint64_t uid,std::shared_ptr<AnimatedTexture>animated_texture,float width,float height,float layer){
    AnimatedTextureComponent temp(animated_texture,width,height,layer,uid);
    m_AnimatedTextureComponents.AddComponent(temp,uid);
}

template<>
void Scene::AddComponent<LightComponent,float,float,float,float,Vec3,LightType>(uint64_t uid,float x_offset,float y_offset,float radius,float blur,Vec3 color,LightType type){
    LightComponent temp(x_offset,y_offset,radius,blur,color,type,uid);
    m_LightComponents.AddComponent(temp,uid);
}   

template<>
void Scene::AddComponent<RigidbodyComponent,RigidbodyComponent::BodyType,bool>(uint64_t uid,RigidbodyComponent::BodyType body_type,bool fixed_rotation){
    RigidbodyComponent temp;
    temp.m_BodyType=body_type;
    temp.m_FixedRotation=fixed_rotation;
    temp.m_UID=uid;
    m_RigidbodyComponents.AddComponent(temp,uid);
}

template<>
void Scene::AddComponent<BoxColliderComponent,float,float,float,float,float,float,float,float>(
        uint64_t uid,float x_offset,float y_offset,float width,float height,float density,float friction,
        float restitution,float restitution_threshold){
    
    BoxColliderComponent temp;
    temp.m_XOffset=x_offset;
    temp.m_YOffset=y_offset;
    temp.m_Width=width;
    temp.m_Height=height;
    temp.m_Density=density;
    temp.m_Friction=friction;
    temp.m_Restitution=restitution;
    temp.m_RestitutionThreshold=restitution_threshold;
    temp.m_UID=uid;
    m_BoxColliderComponents.AddComponent(temp,uid);
}

template<>
void Scene::AddComponent<CircleColliderComponent,float,float,float,float,float,float,float>(
        uint64_t uid,float x_offset,float y_offset,float radius,float density,float friction,
        float restitution,float restitution_threshold){
    
    CircleColliderComponent temp;
    temp.m_XOffset=x_offset;
    temp.m_YOffset=y_offset;
    temp.m_Radius=radius;
    temp.m_Density=density;
    temp.m_Friction=friction;
    temp.m_Restitution=restitution;
    temp.m_RestitutionThreshold=restitution_threshold;
    temp.m_UID=uid;
    m_CircleColliderComponents.AddComponent(temp,uid);
}

template<>
void Scene::AddComponent<NativeScriptComponent>(uint64_t uid){
    NativeScriptComponent temp(uid);
    m_NativeScriptComponents.AddComponent(temp,uid);
}

template<>
void Scene::RemoveComponent<TextureComponent>(uint64_t uid){
    m_TextureComponents.RemoveComponent(uid);
}

template<>
void Scene::RemoveComponent<AnimatedTextureComponent>(uint64_t uid){
    m_AnimatedTextureComponents.RemoveComponent(uid);
}

template<>
void Scene::RemoveComponent<LightComponent>(uint64_t uid){
    m_LightComponents.RemoveComponent(uid);
}

template<>
void Scene::RemoveComponent<RigidbodyComponent>(uint64_t uid){
    m_RigidbodyComponents.RemoveComponent(uid);
}

template<> 
void Scene::RemoveComponent<BoxColliderComponent>(uint64_t uid){
    m_BoxColliderComponents.RemoveComponent(uid);
}

template<>
void Scene::RemoveComponent<CircleColliderComponent>(uint64_t uid){
    m_CircleColliderComponents.RemoveComponent(uid);
}

template<>
void Scene::RemoveComponent<NativeScriptComponent>(uint64_t uid){
    m_NativeScriptComponents.RemoveComponent(uid);
}

template<>
TextureComponent* Scene::GetComponent<TextureComponent>(uint64_t uid){
    return m_TextureComponents.GetComponent(uid);
}

template<>
AnimatedTextureComponent* Scene::GetComponent<AnimatedTextureComponent>(uint64_t uid){
    return m_AnimatedTextureComponents.GetComponent(uid);
}

template<>
LightComponent* Scene::GetComponent<LightComponent>(uint64_t uid){
    return m_LightComponents.GetComponent(uid);
}

template<>
RigidbodyComponent* Scene::GetComponent<RigidbodyComponent>(uint64_t uid){
    return m_RigidbodyComponents.GetComponent(uid);
}

template<>
BoxColliderComponent* Scene::GetComponent<BoxColliderComponent>(uint64_t uid){
    return m_BoxColliderComponents.GetComponent(uid);
}

template<>
CircleColliderComponent* Scene::GetComponent<CircleColliderComponent>(uint64_t uid){
    return m_CircleColliderComponents.GetComponent(uid);
}

template<>
NativeScriptComponent* Scene::GetComponent<NativeScriptComponent>(uint64_t uid){
    return m_NativeScriptComponents.GetComponent(uid);
}

void Scene::Update(double frame_time){
    m_NativeScriptComponents.Update(this,frame_time);
    OnPhysicsUpdate(frame_time);
}

void Scene::Render(){
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

void Scene::MoveEntity(uint64_t uid,float x_offset,float y_offset){
    RigidbodyComponent *rigidbody=GetComponent<RigidbodyComponent>(uid);
    if(rigidbody==nullptr){
        printf("Entity %llu does not have a rigidbody\n",uid);
        return;
    }
    Entity *entity=GetEntity(uid);

    rigidbody->m_RuntimeBody->SetLinearVelocity(b2Vec2(x_offset*m_ScalingFactor,y_offset*m_ScalingFactor));
}

void Scene::OnPhysicsStart(){
    m_PhysicsWorld=new b2World(b2Vec2(0.0f,-0.3f));

    std::vector<RigidbodyComponent> &rigidbodies=m_RigidbodyComponents.m_Components;
    Entity *entity;
    BoxColliderComponent *box_collider;
    CircleColliderComponent *circle_collider;
    for(int i=0;i<rigidbodies.size();i++){
        entity=GetEntity(rigidbodies[i].m_UID);

        b2BodyDef body_def;
        body_def.type=RigidbodyTypeToBox2DBody(rigidbodies[i].m_BodyType);
        body_def.position.Set(entity->m_X*m_ScalingFactor,entity->m_Y*m_ScalingFactor);
        body_def.angle=0.0f;

        b2Body *body=m_PhysicsWorld->CreateBody(&body_def);
        body->SetFixedRotation(rigidbodies[i].m_FixedRotation);
        rigidbodies[i].m_RuntimeBody=body;

        if((box_collider=GetComponent<BoxColliderComponent>(entity->m_UID))!=nullptr){
            b2PolygonShape box_shape;
            box_shape.SetAsBox(box_collider->m_Width/2.0f*m_ScalingFactor,box_collider->m_Height/2.0f*m_ScalingFactor,b2Vec2(box_collider->m_XOffset*m_ScalingFactor,box_collider->m_YOffset*m_ScalingFactor),0.0f);
        
            b2FixtureDef fixture_def;
            fixture_def.shape=&box_shape;
            fixture_def.density=box_collider->m_Density;
            fixture_def.friction=box_collider->m_Friction;
            fixture_def.restitution=box_collider->m_Restitution;
            fixture_def.restitutionThreshold=box_collider->m_RestitutionThreshold;
            body->CreateFixture(&fixture_def);
        }
        if((circle_collider=GetComponent<CircleColliderComponent>(entity->m_UID))!=nullptr){
            b2CircleShape circle_shape;
            circle_shape.m_p.Set((circle_collider->m_XOffset)*m_ScalingFactor,(circle_collider->m_YOffset)*m_ScalingFactor);
            circle_shape.m_radius=(circle_collider->m_Radius)*m_ScalingFactor;

            b2FixtureDef fixture_def;
            fixture_def.shape=&circle_shape;
            fixture_def.density=circle_collider->m_Density;
            fixture_def.friction=circle_collider->m_Friction;
            fixture_def.restitution=circle_collider->m_Restitution;
            fixture_def.restitutionThreshold=circle_collider->m_RestitutionThreshold;
            body->CreateFixture(&fixture_def);
        }
    }
}

void Scene::OnPhysicsUpdate(double frame_time){
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