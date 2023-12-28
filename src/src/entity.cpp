#include <entity.hpp>
#include <cmath>
#include <renderer.hpp>
#include <application.hpp>
#include <global.hpp>

uint64_t NEXT_UID=0;

Entity::Entity(): m_X(0.0f),m_Y(0.0f),m_UID(NEXT_UID++),m_HasCollider(false){}
Entity::Entity(float x,float y): m_X(x),m_Y(y),m_UID(NEXT_UID++),m_HasCollider(false){}

Entity::Entity(Entity &other){
    m_X=other.m_X;
    m_Y=other.m_Y;
    m_UID=other.m_UID;
    m_HasCollider=other.m_HasCollider;
}

Entity::Entity(Entity &&other){
    m_X=other.m_X;
    m_Y=other.m_Y;
    m_UID=other.m_UID;
    m_HasCollider=other.m_HasCollider;
}

Entity &Entity::operator=(Entity &other){
    m_X=other.m_X;
    m_Y=other.m_Y;
    m_UID=other.m_UID;
    m_HasCollider=other.m_HasCollider;
    return *this;
}

Entity &Entity::operator=(Entity &&other){
    m_X=other.m_X;
    m_Y=other.m_Y;
    m_UID=other.m_UID;
    m_HasCollider=other.m_HasCollider;
    return *this;
}

TextureComponent::TextureComponent(const std::string &path,int mag_filter,int min_filter,float width,float height,float layer,uint64_t uid):
    m_Texture(path,mag_filter,min_filter),m_Width(width),m_Height(height),m_Layer(layer),m_UID(uid){}

AnimatedTextureComponent::AnimatedTextureComponent(const std::string &path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter,float width,float height,float layer,uint64_t uid):
    m_AnimatedTexture(path,tile_width,tile_height,mag_filter,min_filter),m_Width(width),m_Height(height),m_Layer(layer),m_UID(uid){}

ColliderComponent::ColliderComponent(float width,float height,float hspeed,float vspeed,float xoffset,float yoffset,uint64_t uid):
    m_Width(width),m_Height(height),m_HSpeed(hspeed),m_VSpeed(vspeed),m_XOffset(xoffset),m_YOffset(yoffset),m_UID(uid){}

bool ColliderComponent::RayVsRect(const Vec2 &ray_origin, const Vec2 &ray_dir,const Rect *target,Vec2 &contact_point,Vec2 &contact_normal,float& time_hit_near){
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

    time_hit_near=std::max(t_near.x,t_near.y);
    float t_hit_far=std::min(t_far.x,t_far.y);

    if(t_hit_far<0)
        return false;

    contact_point=ray_origin+time_hit_near*ray_dir;

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

bool ColliderComponent::DynamicRectVsRect(const Rect *dynamic_rect,const float fTimeStep,const Rect *static_rect,
    Vec2 &contact_point, Vec2 &contact_normal, float& contact_time){
    
    if(dynamic_rect->vel.x==0 && dynamic_rect->vel.y==0)
        return false;

    Rect expanded_target;
    expanded_target.pos=static_rect->pos-dynamic_rect->size/2;
    expanded_target.size=static_rect->size+dynamic_rect->size;

    if(RayVsRect(dynamic_rect->pos+dynamic_rect->size/2,dynamic_rect->vel*fTimeStep,&expanded_target,contact_point,contact_normal,contact_time))
        return (contact_time>=0.0f && contact_time<1.0f);
    else
        return false;
}

bool ColliderComponent::ResolveDynamicRectVsRect(Rect *dynamic_rect,const float fTimeStep,Rect *static_rect){
    Vec2 contact_point,contact_normal;
    float contact_time=0.0f;
    if(DynamicRectVsRect(dynamic_rect,fTimeStep,static_rect,contact_point,contact_normal,contact_time)){
        dynamic_rect->vel=dynamic_rect->vel+contact_normal*Vec2(std::abs(dynamic_rect->vel.x),std::abs(dynamic_rect->vel.y))*(1-contact_time);
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
    m_UID=other.m_UID;
}

TextureComponent::TextureComponent(TextureComponent &&other){
    m_Texture=std::move(other.m_Texture);
    m_Width=other.m_Width;
    m_Height=other.m_Height;
    m_UID=other.m_UID;
}

AnimatedTextureComponent::AnimatedTextureComponent(AnimatedTextureComponent &other){
    m_AnimatedTexture=other.m_AnimatedTexture;
    m_Width=other.m_Width;
    m_Height=other.m_Height;
    m_UID=other.m_UID;
}

AnimatedTextureComponent::AnimatedTextureComponent(AnimatedTextureComponent &&other){
    m_AnimatedTexture=std::move(other.m_AnimatedTexture);
    m_Width=other.m_Width;
    m_Height=other.m_Height;
    m_UID=other.m_UID;
} 

void AnimatedTextureComponent::PlayAnimation(bool loop,float delay){
    m_AnimatedTexture.PlayAnimation(loop,delay);
}

LightComponent::LightComponent(): m_XOffset(0.0f),m_YOffset(0.0f),m_Radius(0.0f),m_Blur(0.0f),m_Color(0.0f,0.0f,0.0f),m_Type(LIGHT_AROUND_POS),m_UID(std::numeric_limits<uint64_t>::max()){}
LightComponent::LightComponent(float x_offset,float y_offset,float radius,float blur,Vec3 color,LightType type,uint64_t uid): 
    m_XOffset(x_offset),m_YOffset(y_offset),m_Radius(radius),m_Blur(blur),m_Color(color),m_Type(type),m_UID(uid){}

void LightComponent::SetOffset(float x_offset,float y_offset){
    m_XOffset=x_offset;
    m_YOffset=y_offset;
}

void LightComponent::SetCentered(float width,float height){
    m_XOffset=width/2;
    m_YOffset=height/2;
}

template<>
void ComponentManager<TextureComponent>::Update(float frame_time,std::vector<Entity>& entities){
    printf("Texture components don't need to be updated!\n");
}

template<>
void ComponentManager<AnimatedTextureComponent>::Update(float frame_time,std::vector<Entity>& entities){
    printf("Animated texture components don't need to be updated!\n");
}

template<>
void ComponentManager<ColliderComponent>::Update(float frame_time,std::vector<Entity>& entities){
    Entity *entity=nullptr;
    Entity *temp_entity=nullptr;
    for(int i=0;i<m_Components.size();i++){
        Vec2 cp,cn;
        float contact_time=0;
        std::vector<std::pair<int,float>>z;
        Rect *rect=GetComponentAsRect(i);
        entity=BinarySearch(entities,m_Components[i].m_UID);
        rect->pos.x+=entity->m_X;
        rect->pos.y+=entity->m_Y;

        for(int j=0;j<m_Components.size();j++){
            Rect *temp=GetComponentAsRect(j);
            temp_entity=BinarySearch(entities,m_Components[j].m_UID);
            temp->pos.x+=temp_entity->m_X;
            temp->pos.y+=temp_entity->m_Y;
            if(ColliderComponent::DynamicRectVsRect(rect,frame_time,temp,cp,cn,contact_time))
                z.push_back({j,contact_time});
            delete temp;
        }

        std::sort(begin(z),end(z),[](const std::pair<int,float> &a,const std::pair<int,float> &b){
            return a.second<b.second;
        });

        for(auto j:z){
            Rect *temp=GetComponentAsRect(j.first);
            temp_entity=BinarySearch(entities,m_Components[j.first].m_UID);
            temp->pos.x+=temp_entity->m_X;
            temp->pos.y+=temp_entity->m_Y;
            ColliderComponent::ResolveDynamicRectVsRect(rect,frame_time,temp);
            delete temp;
        }
        rect->pos=rect->pos+rect->vel*frame_time;
        entity->m_X=rect->pos.x-m_Components[i].m_XOffset;
        entity->m_Y=rect->pos.y-m_Components[i].m_YOffset;
        m_Components[i].m_HSpeed=0;
        m_Components[i].m_VSpeed=0;
        delete rect;
        z.clear();
    }
}

template<>
void ComponentManager<LightComponent>::Update(float frame_time,std::vector<Entity>& entities){
    printf("Light components don't need to be updated!\n");
}

template<>
void ComponentManager<TextureComponent>::Render(std::vector<Entity>& entities){
    for(int i=0;i<m_Components.size();i++){
        Entity *entity=BinarySearch(entities,m_Components[i].m_UID);
        RENDERER->DrawTexture(entity->m_X,entity->m_Y,m_Components[i].m_Width,m_Components[i].m_Height,false,false,m_Components[i].m_Layer,m_Components[i].m_Texture);
    }
}

template<>
void ComponentManager<AnimatedTextureComponent>::Render(std::vector<Entity>& entities){
    Entity *entity=nullptr;
    for(int i=0;i<m_Components.size();i++){
        entity=BinarySearch(entities,m_Components[i].m_UID);
        RENDERER->DrawAnimatedTexture(entity->m_X,entity->m_Y,m_Components[i].m_Width,m_Components[i].m_Height,m_Components[i].m_Layer,m_Components[i].m_AnimatedTexture);  
        RENDERER->DrawLine(entity->m_X,entity->m_Y,entity->m_X+m_Components[i].m_Width,entity->m_Y,Vec4(0.0f,1.0f,0.0f,1.0f),-2);
        RENDERER->DrawLine(entity->m_X+m_Components[i].m_Width,entity->m_Y,entity->m_X+m_Components[i].m_Width,entity->m_Y+m_Components[i].m_Height,Vec4(0.0f,1.0f,0.0f,1.0f),-2);
        RENDERER->DrawLine(entity->m_X+m_Components[i].m_Width,entity->m_Y+m_Components[i].m_Height,entity->m_X,entity->m_Y+m_Components[i].m_Height,Vec4(0.0f,1.0f,0.0f,1.0f),-2);
        RENDERER->DrawLine(entity->m_X,entity->m_Y+m_Components[i].m_Height,entity->m_X,entity->m_Y,Vec4(0.0f,1.0f,0.0f,1.0f),-2);
    }
}

template<>
void ComponentManager<ColliderComponent>::Render(std::vector<Entity>& entities){
    Entity *entity=nullptr;
    for(int i=0;i<m_Components.size();i++){
        entity=BinarySearch(entities,m_Components[i].m_UID);
        RENDERER->DrawLine(entity->m_X,entity->m_Y,entity->m_X+m_Components[i].m_Width,entity->m_Y,Vec4(1.0f,0.0f,0.0f,1.0f),-2);
        RENDERER->DrawLine(entity->m_X+m_Components[i].m_Width,entity->m_Y,entity->m_X+m_Components[i].m_Width,entity->m_Y+m_Components[i].m_Height,Vec4(1.0f,0.0f,0.0f,1.0f),-2);
        RENDERER->DrawLine(entity->m_X+m_Components[i].m_Width,entity->m_Y+m_Components[i].m_Height,entity->m_X,entity->m_Y+m_Components[i].m_Height,Vec4(1.0f,0.0f,0.0f,1.0f),-2);
        RENDERER->DrawLine(entity->m_X,entity->m_Y+m_Components[i].m_Height,entity->m_X,entity->m_Y,Vec4(1.0f,0.0f,0.0f,1.0f),-2);
    }
}

template<>
void ComponentManager<LightComponent>::Render(std::vector<Entity>& entities){
    Entity *entity=nullptr;
    for(int i=0;i<m_Components.size();i++){
        entity=BinarySearch(entities,m_Components[i].m_UID);
        RENDERER->DrawLight(entity->m_X+m_Components[i].m_XOffset,entity->m_Y+m_Components[i].m_YOffset,Vec4(m_Components[i].m_Color.r,m_Components[i].m_Color.g,m_Components[i].m_Color.b,1.0f),m_Components[i].m_Type,m_Components[i].m_Radius,m_Components[i].m_Blur);
    }
}