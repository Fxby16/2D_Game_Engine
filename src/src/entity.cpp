#include <entity.hpp>
#include <cmath>
#include <renderer.hpp>
#include <game.hpp>
#include <global.hpp>

uint64_t NEXT_UID=0;

ComponentManager<TextureComponent> CMTC;
ComponentManager<AnimatedTextureComponent> CMATC;
ComponentManager<ColliderComponent> CMCC;

Entity::Entity(): m_X(0.0f),m_Y(0.0f),m_UID(NEXT_UID++),m_HasCollider(false){}
Entity::Entity(float x,float y): m_X(x),m_Y(y),m_UID(NEXT_UID++),m_HasCollider(false){}

Entity::~Entity(){
    CMTC.RemoveComponent(m_UID);
    CMATC.RemoveComponent(m_UID);
    CMCC.RemoveComponent(m_UID);
}

void Entity::Move(float x_offset,float y_offset){
    if(m_HasCollider){
        ColliderComponent *cc=CMCC.GetComponent(m_UID);
        cc->Move(x_offset,y_offset);
    }else{
        m_X+=x_offset*DELTA_TIME;
        m_Y+=y_offset*DELTA_TIME;
    }
}

void Entity::SetPos(float x,float y){
    m_X=x;
    m_Y=y;
}

template<>
void Entity::AddComponent<TextureComponent,const char*,int,int,float,float,int>(const char *path,int mag_filter,int min_filter,float width,float height,int depth){
    TextureComponent temp(path,mag_filter,min_filter,width,height,depth,this);
    CMTC.AddComponent(temp,m_UID);
}

template<>
void Entity::AddComponent<AnimatedTextureComponent,const char*,unsigned int,unsigned int,int,int,float,float,int>(const char *path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter,float width,float height,int depth){
    AnimatedTextureComponent temp(path,tile_width,tile_height,mag_filter,min_filter,width,height,depth,this);
    CMATC.AddComponent(temp,m_UID);
}

template<>
void Entity::AddComponent<ColliderComponent,float,float,float,float,float,float>(float width,float height,float hspeed,float vspeed,float xoffset,float yoffset){
    m_HasCollider=true;
    ColliderComponent temp(width,height,hspeed,vspeed,xoffset,yoffset,this);
    CMCC.AddComponent(temp,m_UID);
}

template<>
void Entity::RemoveComponent<TextureComponent>(){
    CMTC.RemoveComponent(m_UID);
}

template<>
void Entity::RemoveComponent<AnimatedTextureComponent>(){
    CMATC.RemoveComponent(m_UID);
}

template<>
void Entity::RemoveComponent<ColliderComponent>(){
    CMCC.RemoveComponent(m_UID);
}

template<>
TextureComponent* Entity::GetComponent<TextureComponent>(){
    return CMTC.GetComponent(m_UID);
}

template<>
AnimatedTextureComponent* Entity::GetComponent<AnimatedTextureComponent>(){
    return CMATC.GetComponent(m_UID);
}

template<>
ColliderComponent* Entity::GetComponent<ColliderComponent>(){
    return CMCC.GetComponent(m_UID);
}

TextureComponent::TextureComponent(const std::string &path,int mag_filter,int min_filter,float width,float height,int depth,Entity *entity):
    m_Texture(path,mag_filter,min_filter),m_Width(width),m_Height(height),m_Depth(depth),m_Entity(entity){}

AnimatedTextureComponent::AnimatedTextureComponent(const std::string &path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter,float width,float height,int depth,Entity *entity):
    m_AnimatedTexture(path,tile_width,tile_height,mag_filter,min_filter),m_Width(width),m_Height(height),m_Depth(depth),m_Entity(entity){}

ColliderComponent::ColliderComponent(float width,float height,float hspeed,float vspeed,float xoffset,float yoffset,Entity *entity):
    m_Width(width),m_Height(height),m_HSpeed(hspeed),m_VSpeed(vspeed),m_XOffset(xoffset),m_YOffset(yoffset),m_Entity(entity){}

bool ColliderComponent::RayVsRect(const Vec2 &ray_origin, const Vec2 &ray_dir,const Rect *target,Vec2 &contact_point,Vec2 &contact_normal,float& t_hit_near){
    contact_normal={0,0};
    contact_point={0,0};

    Vec2 invdir=1.0f/ray_dir;

    Vec2 t_near=(target->pos-ray_origin)*invdir;
    Vec2 t_far=(target->pos+target->size-ray_origin)*invdir;

    if(std::isnan(t_far.y) || std::isnan(t_far.x)) return false;
    if(std::isnan(t_near.y) || std::isnan(t_near.x)) return false;

    if(t_near.x>t_far.x) std::swap(t_near.x,t_far.x);
    if(t_near.y>t_far.y) std::swap(t_near.y,t_far.y);

    if(t_near.x>t_far.y || t_near.y>t_far.x) return false;

    t_hit_near=std::max(t_near.x,t_near.y);
    float t_hit_far=std::min(t_far.x,t_far.y);

    if(t_hit_far<0)
        return false;

    contact_point=ray_origin+t_hit_near*ray_dir;

    if(t_near.x>t_near.y){
        if(invdir.x<0)
            contact_normal={1,0};
        else
            contact_normal={-1,0};
    }else if(t_near.x<t_near.y){
        if(invdir.y<0)
            contact_normal={0,1};
        else
            contact_normal={0,-1};
    }

    return true;
}

bool ColliderComponent::DynamicRectVsRect(const Rect* r_dynamic,const float fTimeStep,const Rect* r_static,
    Vec2 &contact_point, Vec2 &contact_normal, float& contact_time){
    
    if(r_dynamic->vel.x==0 && r_dynamic->vel.y==0)
        return false;

    Rect expanded_target;
    expanded_target.pos=r_static->pos-r_dynamic->size/2;
    expanded_target.size=r_static->size+r_dynamic->size;

    if(RayVsRect(r_dynamic->pos+r_dynamic->size/2,r_dynamic->vel*fTimeStep,&expanded_target,contact_point,contact_normal,contact_time))
        return (contact_time>=0.0f && contact_time<1.0f);
    else
        return false;
}

bool ColliderComponent::ResolveDynamicRectVsRect(Rect* r_dynamic,const float fTimeStep,Rect* r_static){
    Vec2 contact_point,contact_normal;
    float contact_time=0.0f;
    if(DynamicRectVsRect(r_dynamic,fTimeStep,r_static,contact_point,contact_normal,contact_time)){
        r_dynamic->vel=r_dynamic->vel+contact_normal*Vec2(std::abs(r_dynamic->vel.x),std::abs(r_dynamic->vel.y))*(1-contact_time);
        return true;
    }

    return false;
}

void ColliderComponent::Move(float x_offset,float y_offset){
    m_HSpeed+=x_offset;
    m_VSpeed+=y_offset;
}

TextureComponent::TextureComponent(TextureComponent &other){
    m_Texture=other.m_Texture;
    m_Width=other.m_Width;
    m_Height=other.m_Height;
    m_Entity=other.m_Entity;
}

TextureComponent::TextureComponent(TextureComponent &&other){
    m_Texture=std::move(other.m_Texture);
    m_Width=other.m_Width;
    m_Height=other.m_Height;
    m_Entity=other.m_Entity;
}

AnimatedTextureComponent::AnimatedTextureComponent(AnimatedTextureComponent &other){
    m_AnimatedTexture=other.m_AnimatedTexture;
    m_Width=other.m_Width;
    m_Height=other.m_Height;
    m_Entity=other.m_Entity;
}

AnimatedTextureComponent::AnimatedTextureComponent(AnimatedTextureComponent &&other){
    m_AnimatedTexture=std::move(other.m_AnimatedTexture);
    m_Width=other.m_Width;
    m_Height=other.m_Height;
    m_Entity=other.m_Entity;
} 

template<>
void ComponentManager<TextureComponent>::Update(float frame_time){
    printf("Texture components don't need to be updated!\n");
}

template<>
void ComponentManager<AnimatedTextureComponent>::Update(float frame_time){
    printf("Animated texture components don't need to be updated!\n");
}

template<>
void ComponentManager<ColliderComponent>::Update(float frame_time){
    for(int i=0;i<m_Components.size();i++){
        Vec2 cp,cn;
        float contact_time=0;
        std::vector<std::pair<int,float>>z;
        Rect *rect=GetComponentAsRect(i);

        for(int j=0;j<m_Components.size();j++){
            Rect *temp=GetComponentAsRect(j);
            if(ColliderComponent::DynamicRectVsRect(rect,frame_time,temp,cp,cn,contact_time))
                z.push_back({j,contact_time});
            delete temp;
        }

        std::sort(begin(z),end(z),[](const std::pair<int,float> &a,const std::pair<int,float> &b){
            return a.second<b.second;
        });

        for(auto j:z){
            Rect *temp=GetComponentAsRect(j.first);
            ColliderComponent::ResolveDynamicRectVsRect(rect,frame_time,temp);
            delete temp;
        }
        rect->pos=rect->pos+rect->vel*frame_time;
        m_Components[i].m_Entity->m_X=rect->pos.x-m_Components[i].m_XOffset;
        m_Components[i].m_Entity->m_Y=rect->pos.y-m_Components[i].m_YOffset;
        m_Components[i].m_HSpeed=0;
        m_Components[i].m_VSpeed=0;
        delete rect;
        z.clear();
    }
}

template<>
void ComponentManager<TextureComponent>::Render(){
    for(int i=0;i<m_Components.size();i++){
        m_Renderer->DrawTexture(m_Components[i].m_Entity->m_X,m_Components[i].m_Entity->m_Y,m_Components[i].m_Width,m_Components[i].m_Height,false,false,m_Components[i].m_Depth,m_Components[i].m_Texture);
    }
}

template<>
void ComponentManager<AnimatedTextureComponent>::Render(){
    for(int i=0;i<m_Components.size();i++){
        m_Renderer->DrawAnimatedTexture(m_Components[i].m_Entity->m_X,m_Components[i].m_Entity->m_Y,m_Components[i].m_Width,m_Components[i].m_Height,m_Components[i].m_Depth,m_Components[i].m_AnimatedTexture);  
        m_Renderer->DrawLine(m_Components[i].m_Entity->m_X,m_Components[i].m_Entity->m_Y,m_Components[i].m_Entity->m_X+m_Components[i].m_Width,m_Components[i].m_Entity->m_Y,0.0f,1.0f,0.0f,1.0f);
        m_Renderer->DrawLine(m_Components[i].m_Entity->m_X+m_Components[i].m_Width,m_Components[i].m_Entity->m_Y,m_Components[i].m_Entity->m_X+m_Components[i].m_Width,m_Components[i].m_Entity->m_Y+m_Components[i].m_Height,0.0f,1.0f,0.0f,1.0f);
        m_Renderer->DrawLine(m_Components[i].m_Entity->m_X+m_Components[i].m_Width,m_Components[i].m_Entity->m_Y+m_Components[i].m_Height,m_Components[i].m_Entity->m_X,m_Components[i].m_Entity->m_Y+m_Components[i].m_Height,0.0f,1.0f,0.0f,1.0f);
        m_Renderer->DrawLine(m_Components[i].m_Entity->m_X,m_Components[i].m_Entity->m_Y+m_Components[i].m_Height,m_Components[i].m_Entity->m_X,m_Components[i].m_Entity->m_Y,0.0f,1.0f,0.0f,1.0f);
    }
}

template<>
void ComponentManager<ColliderComponent>::Render(){
    for(int i=0;i<m_Components.size();i++){
        m_Renderer->DrawLine(m_Components[i].m_Entity->m_X,m_Components[i].m_Entity->m_Y,m_Components[i].m_Entity->m_X+m_Components[i].m_Width,m_Components[i].m_Entity->m_Y,1.0f,0.0f,0.0f,1.0f);
        m_Renderer->DrawLine(m_Components[i].m_Entity->m_X+m_Components[i].m_Width,m_Components[i].m_Entity->m_Y,m_Components[i].m_Entity->m_X+m_Components[i].m_Width,m_Components[i].m_Entity->m_Y+m_Components[i].m_Height,1.0f,0.0f,0.0f,1.0f);
        m_Renderer->DrawLine(m_Components[i].m_Entity->m_X+m_Components[i].m_Width,m_Components[i].m_Entity->m_Y+m_Components[i].m_Height,m_Components[i].m_Entity->m_X,m_Components[i].m_Entity->m_Y+m_Components[i].m_Height,1.0f,0.0f,0.0f,1.0f);
        m_Renderer->DrawLine(m_Components[i].m_Entity->m_X,m_Components[i].m_Entity->m_Y+m_Components[i].m_Height,m_Components[i].m_Entity->m_X,m_Components[i].m_Entity->m_Y,1.0f,0.0f,0.0f,1.0f);
    }
}