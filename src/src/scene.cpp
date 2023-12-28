#include <scene.hpp>

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
    m_ColliderComponents.RemoveComponent(uid);
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
void Scene::AddComponent<AnimatedTextureComponent,const char*,unsigned int,unsigned int,int,int,float,float,float>(uint64_t uid,const char *path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter,float width,float height,float layer){
    AnimatedTextureComponent temp(path,tile_width,tile_height,mag_filter,min_filter,width,height,layer,uid);
    m_AnimatedTextureComponents.AddComponent(temp,uid);    
}

template<>
void Scene::AddComponent<ColliderComponent,float,float,float,float,float,float>(uint64_t uid,float width,float height,float hspeed,float vspeed,float xoffset,float yoffset){
    Entity *entity=GetEntity(uid);
    entity->m_HasCollider=true;
    ColliderComponent temp(width,height,hspeed,vspeed,xoffset,yoffset,uid);
    m_ColliderComponents.AddComponent(temp,uid);
}

template<>
void Scene::AddComponent<LightComponent,float,float,float,float,Vec3,LightType>(uint64_t uid,float x_offset,float y_offset,float radius,float blur,Vec3 color,LightType type){
    LightComponent temp(x_offset,y_offset,radius,blur,color,type,uid);
    m_LightComponents.AddComponent(temp,uid);
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
void Scene::RemoveComponent<ColliderComponent>(uint64_t uid){
    m_ColliderComponents.RemoveComponent(uid);
}

template<>
void Scene::RemoveComponent<LightComponent>(uint64_t uid){
    m_LightComponents.RemoveComponent(uid);
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
ColliderComponent* Scene::GetComponent<ColliderComponent>(uint64_t uid){
    return m_ColliderComponents.GetComponent(uid);
}

template<>
LightComponent* Scene::GetComponent<LightComponent>(uint64_t uid){
    return m_LightComponents.GetComponent(uid);
}

void Scene::Update(double frame_time){
    m_ColliderComponents.Update(frame_time,m_Entities);
}

void Scene::Render(){
    m_TextureComponents.Render(m_Entities);
    m_AnimatedTextureComponents.Render(m_Entities);
    m_ColliderComponents.Render(m_Entities);
    m_LightComponents.Render(m_Entities);
}

void SceneManager::AddScene(const std::string &name){
    m_Scenes.push_back(Scene(name));
}

void SceneManager::RemoveScene(const std::string &name){
    if(name==m_CurrentSceneName)
        m_CurrentSceneName="";
    for(auto &scene:m_Scenes){
        if(scene.m_Name==name){
            scene.m_Name="";
            break;
        }
    }
}

void SceneManager::SetCurrentScene(const std::string &name){
    m_CurrentSceneName=name;
}

Scene* SceneManager::GetCurrentScene(){
    for(auto &scene:m_Scenes){
        if(scene.m_Name==m_CurrentSceneName)
            return &scene;
    }
    return nullptr;
}

void Scene::MoveEntity(uint64_t uid,float x_offset,float y_offset){
    Entity *entity=GetEntity(uid);
    if(entity==nullptr)
        return;
    if(entity->m_HasCollider){
        ColliderComponent *collider=GetComponent<ColliderComponent>(uid);
        if(collider!=nullptr){
            collider->m_HSpeed+=x_offset;
            collider->m_VSpeed+=y_offset;
        }
    }else{
        entity->m_X+=x_offset*DELTA_TIME;
        entity->m_Y+=y_offset*DELTA_TIME;
    }
}