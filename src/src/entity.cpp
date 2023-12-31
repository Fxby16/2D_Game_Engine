#include <entity.hpp>
#include <cmath>
#include <renderer.hpp>
#include <application.hpp>
#include <global.hpp>
#include <scene.hpp>
#include <Instrumentor.h>

uint64_t NEXT_UID=0;

Entity::Entity(): m_X(0.0f),m_Y(0.0f),m_PreviousX(0.0f),m_PreviousY(0.0f),m_UID(NEXT_UID++){}
Entity::Entity(float x,float y): m_X(x),m_Y(y),m_PreviousX(x),m_PreviousY(y),m_UID(NEXT_UID++){}

Entity::Entity(Entity &other){
    m_X=other.m_X;
    m_Y=other.m_Y;
    m_PreviousX=other.m_PreviousX;
    m_PreviousY=other.m_PreviousY;
    m_UID=other.m_UID;
}

Entity::Entity(Entity &&other){
    m_X=other.m_X;
    m_Y=other.m_Y;
    m_PreviousX=other.m_PreviousX;
    m_PreviousY=other.m_PreviousY;
    m_UID=other.m_UID;
}

Entity &Entity::operator=(Entity &other){
    m_X=other.m_X;
    m_Y=other.m_Y;
    m_PreviousX=other.m_PreviousX;
    m_PreviousY=other.m_PreviousY;
    m_UID=other.m_UID;
    return *this;
}

Entity &Entity::operator=(Entity &&other){
    m_X=other.m_X;
    m_Y=other.m_Y;
    m_PreviousX=other.m_PreviousX;
    m_PreviousY=other.m_PreviousY;
    m_UID=other.m_UID;
    return *this;
}

TextureComponent::TextureComponent(const std::string &path,int mag_filter,int min_filter,float width,float height,float layer,uint64_t uid):
    m_Texture(std::make_shared<Texture>(path,mag_filter,min_filter)),m_Width(width),m_Height(height),m_Layer(layer),m_UID(uid){}

TextureComponent::TextureComponent(std::shared_ptr<Texture>&t,float width,float height,float layer,uint64_t uid):
    m_Texture(t),m_Width(width),m_Height(height),m_Layer(layer),m_UID(uid){}

AnimatedTextureComponent::AnimatedTextureComponent(const std::string &path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter,float width,float height,float layer,uint64_t uid):
    m_AnimatedTexture(std::make_shared<AnimatedTexture>(path,tile_width,tile_height,mag_filter,min_filter)),m_Width(width),m_Height(height),m_Layer(layer),m_UID(uid){}

AnimatedTextureComponent::AnimatedTextureComponent(std::shared_ptr<AnimatedTexture>&t,float width,float height,float layer,uint64_t uid):
    m_AnimatedTexture(t),m_Width(width),m_Height(height),m_Layer(layer),m_UID(uid){}

TextureComponent::TextureComponent(TextureComponent &other){
    m_Texture=other.m_Texture;
    m_Width=other.m_Width;
    m_Height=other.m_Height;
    m_UID=other.m_UID;
}

TextureComponent::TextureComponent(TextureComponent &&other){
    m_Texture=other.m_Texture;
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
    m_AnimatedTexture=other.m_AnimatedTexture;
    m_Width=other.m_Width;
    m_Height=other.m_Height;
    m_UID=other.m_UID;
}

void AnimatedTextureComponent::PlayAnimation(bool loop,float delay){
    m_AnimatedTexture->PlayAnimation(loop,delay);
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
void ComponentManager<TextureComponent>::Update(Scene *scene,float frame_time){
    printf("Texture components don't need to be updated!\n");
}

template<>
void ComponentManager<AnimatedTextureComponent>::Update(Scene *scene,float frame_time){
    printf("Animated texture components don't need to be updated!\n");
}

template<>
void ComponentManager<LightComponent>::Update(Scene *scene,float frame_time){
    printf("Light components don't need to be updated!\n");
}

template<>
void ComponentManager<NativeScriptComponent>::Update(Scene *scene,float frame_time){
    PROFILE_FUNCTION();

    for(int i=0;i<m_Components.size();i++){
        if(m_Components[i].OnUpdate){
            m_Components[i].OnUpdate(scene,&m_Components[i],frame_time);
        }
    }
}

template<>
void ComponentManager<TextureComponent>::Render(Scene *scene){
    PROFILE_FUNCTION();

    Entity *entity=nullptr;
    for(int i=0;i<m_Components.size();i++){
        entity=scene->GetEntity(m_Components[i].m_UID);
        RENDERER->DrawTexture({Interpolate(entity->m_X,entity->m_PreviousX),Interpolate(entity->m_Y,entity->m_PreviousY)},{m_Components[i].m_Width,m_Components[i].m_Height},false,false,m_Components[i].m_Layer,*m_Components[i].m_Texture);
    }
}

template<>
void ComponentManager<AnimatedTextureComponent>::Render(Scene *scene){
    PROFILE_FUNCTION();

    Entity *entity=nullptr;
    for(int i=0;i<m_Components.size();i++){
        entity=scene->GetEntity(m_Components[i].m_UID);
        RENDERER->DrawAnimatedTexture({Interpolate(entity->m_X,entity->m_PreviousX),Interpolate(entity->m_Y,entity->m_PreviousY)},{m_Components[i].m_Width,m_Components[i].m_Height},m_Components[i].m_Layer,*m_Components[i].m_AnimatedTexture);  
    }
}

template<>
void ComponentManager<LightComponent>::Render(Scene *scene){
    PROFILE_FUNCTION();

    Entity *entity=nullptr;
    for(int i=0;i<m_Components.size();i++){
        entity=scene->GetEntity(m_Components[i].m_UID);
        RENDERER->DrawLight({Interpolate(entity->m_X,entity->m_PreviousX)+m_Components[i].m_XOffset,Interpolate(entity->m_Y,entity->m_PreviousY)+m_Components[i].m_YOffset},Vec4(m_Components[i].m_Color.r,m_Components[i].m_Color.g,m_Components[i].m_Color.b,1.0f),m_Components[i].m_Type,m_Components[i].m_Radius,m_Components[i].m_Blur);
    }
}