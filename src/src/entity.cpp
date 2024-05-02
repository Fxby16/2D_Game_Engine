#include <pch.hpp>  
#include <entity.hpp>
#include <renderer.hpp>
#include <application.hpp>
#include <global.hpp>
#include <scene.hpp>

uint32_t NEXT_UID=0;

Entity::Entity(uint32_t parent): m_X(0.0f),m_Y(0.0f),m_PreviousX(0.0f),m_PreviousY(0.0f),m_UID(NEXT_UID++),m_Parent(parent){}
Entity::Entity(float x,float y): m_X(x),m_Y(y),m_PreviousX(x),m_PreviousY(y),m_UID(NEXT_UID++),m_Parent(std::numeric_limits<uint32_t>::max()){}
Entity::Entity(uint32_t uid,uint32_t parent): m_X(0.0f),m_Y(0.0f),m_PreviousX(0.0f),m_PreviousY(0.0f),m_UID(uid),m_Parent(parent){}

Entity::Entity(const Entity &other){
    m_X=other.m_X;
    m_Y=other.m_Y;
    m_PreviousX=other.m_PreviousX;
    m_PreviousY=other.m_PreviousY;
    m_UID=other.m_UID;
    m_Parent=other.m_Parent;
}

Entity::Entity(Entity &&other){
    m_X=other.m_X;
    m_Y=other.m_Y;
    m_PreviousX=other.m_PreviousX;
    m_PreviousY=other.m_PreviousY;
    m_UID=other.m_UID;
    m_Parent=other.m_Parent;
}

Entity &Entity::operator=(const Entity &other){
    m_X=other.m_X;
    m_Y=other.m_Y;
    m_PreviousX=other.m_PreviousX;
    m_PreviousY=other.m_PreviousY;
    m_UID=other.m_UID;
    m_Parent=other.m_Parent;
    return *this;
}

Entity &Entity::operator=(Entity &&other){
    m_X=other.m_X;
    m_Y=other.m_Y;
    m_PreviousX=other.m_PreviousX;
    m_PreviousY=other.m_PreviousY;
    m_UID=other.m_UID;
    m_Parent=other.m_Parent;
    return *this;
}

TagComponent::TagComponent(TagComponent &other){
    m_Tag=other.m_Tag;
    m_UID=other.m_UID;
}

TagComponent::TagComponent(TagComponent &&other){
    m_Tag=other.m_Tag;
    m_UID=other.m_UID;
}

TextureComponent::TextureComponent(const std::string &path,int mag_filter,int min_filter,float width,float height,int layer,bool visible,bool flipx,bool flipy,uint32_t uid):
    m_Width(width),m_Height(height),m_Layer(layer),m_Visible(visible),m_FlipX(flipx),m_FlipY(flipy),m_UID(uid){
    
    m_Texture=TEXTURES_MANAGER->GetTexture(path,mag_filter,min_filter).second;
}

TextureComponent::TextureComponent(std::shared_ptr<Texture>t,float width,float height,int layer,bool visible,bool flipx,bool flipy,uint32_t uid):
    m_Texture(t),m_Width(width),m_Height(height),m_Layer(layer),m_Visible(visible),m_FlipX(flipx),m_FlipY(flipy),m_UID(uid){}

AnimatedTextureComponent::AnimatedTextureComponent(const std::string &path,unsigned int tile_width,
    unsigned int tile_height,int mag_filter,int min_filter,float width,float height,int layer,bool play_animation,
    bool loop_animation,float animation_delay,int animation_row,int animation_index,bool visible,bool flipx,bool flipy,uint32_t uid):
    
    m_Width(width),m_Height(height),m_Layer(layer),m_PlayAnimation(play_animation),
    m_LoopAnimation(loop_animation),m_AnimationDelay(animation_delay),m_AnimationRow(animation_row),
    m_AnimationIndex(animation_index),m_LastAnimationTime(0),m_Visible(visible),m_FlipX(flipx),m_FlipY(flipy),m_UID(uid){

    m_AnimatedTexture=TEXTURES_MANAGER->GetSpriteSheet(path,tile_width,tile_height,mag_filter,min_filter).second;
}

AnimatedTextureComponent::AnimatedTextureComponent(std::shared_ptr<SpriteSheet>t,float width,float height,int layer,
    bool play_animation,bool loop_animation,float animation_delay,int animation_row,int animation_index,bool visible,bool flipx,bool flipy,uint32_t uid):
    m_AnimatedTexture(t),m_Width(width),m_Height(height),m_Layer(layer),m_PlayAnimation(play_animation),
    m_LoopAnimation(loop_animation),m_AnimationDelay(animation_delay),m_AnimationRow(animation_row),
    m_AnimationIndex(animation_index),m_LastAnimationTime(0),m_Visible(visible),m_FlipX(flipx),m_FlipY(flipy),m_UID(uid){}

TextureComponent::TextureComponent(TextureComponent &other){
    m_Texture=other.m_Texture;
    m_Width=other.m_Width;
    m_Height=other.m_Height;
    m_Layer=other.m_Layer;
    m_Visible=other.m_Visible;
    m_FlipX=other.m_FlipX;
    m_FlipY=other.m_FlipY;
    m_UID=other.m_UID;
}

TextureComponent::TextureComponent(TextureComponent &&other){
    m_Texture=other.m_Texture;
    m_Width=other.m_Width;
    m_Height=other.m_Height;
    m_Layer=other.m_Layer;
    m_Visible=other.m_Visible;
    m_FlipX=other.m_FlipX;
    m_FlipY=other.m_FlipY;
    m_UID=other.m_UID;
}

AnimatedTextureComponent::AnimatedTextureComponent(AnimatedTextureComponent &other){
    m_AnimatedTexture=other.m_AnimatedTexture;
    m_Width=other.m_Width;
    m_Height=other.m_Height;
    m_Layer=other.m_Layer;
    m_PlayAnimation=other.m_PlayAnimation;
    m_LoopAnimation=other.m_LoopAnimation;
    m_AnimationDelay=other.m_AnimationDelay;
    m_AnimationRow=other.m_AnimationRow;
    m_AnimationIndex=other.m_AnimationIndex;
    m_LastAnimationTime=other.m_LastAnimationTime;
    m_Visible=other.m_Visible;
    m_FlipX=other.m_FlipX;
    m_FlipY=other.m_FlipY;
    m_UID=other.m_UID;
}

AnimatedTextureComponent::AnimatedTextureComponent(AnimatedTextureComponent &&other){
    m_AnimatedTexture=other.m_AnimatedTexture;
    m_Width=other.m_Width;
    m_Height=other.m_Height;
    m_Layer=other.m_Layer;
    m_PlayAnimation=other.m_PlayAnimation;
    m_LoopAnimation=other.m_LoopAnimation;
    m_AnimationDelay=other.m_AnimationDelay;
    m_AnimationRow=other.m_AnimationRow;
    m_AnimationIndex=other.m_AnimationIndex;
    m_LastAnimationTime=other.m_LastAnimationTime;
    m_Visible=other.m_Visible;
    m_FlipX=other.m_FlipX;
    m_FlipY=other.m_FlipY;
    m_UID=other.m_UID;
}

void AnimatedTextureComponent::PlayAnimation(bool loop,float delay){
    m_LoopAnimation=loop;
    m_AnimationDelay=delay;
    m_PlayAnimation=true;
    m_LastAnimationTime=0.0f;
}

LightComponent::LightComponent(): m_XOffset(0.0f),m_YOffset(0.0f),m_Radius(0.0f),m_Blur(0.0f),m_Color(0.0f,0.0f,0.0f),m_Type(LIGHT_AROUND_POS),m_UID(std::numeric_limits<uint32_t>::max()){}
LightComponent::LightComponent(float x_offset,float y_offset,float radius,float blur,Vec3 color,LightType type,uint32_t uid): 
    m_XOffset(x_offset),m_YOffset(y_offset),m_Radius(radius),m_Blur(blur),m_Color(color),m_Type(type),m_UID(uid){}
LightComponent::LightComponent(uint32_t uid): m_XOffset(0.0f),m_YOffset(0.0f),m_Radius(0.0f),m_Blur(0.0f),m_Color(0.0f,0.0f,0.0f),m_Type(LIGHT_AROUND_POS),m_UID(uid){}

void LightComponent::SetOffset(float x_offset,float y_offset){
    m_XOffset=x_offset;
    m_YOffset=y_offset;
}

void LightComponent::SetCentered(float width,float height){
    m_XOffset=width/2;
    m_YOffset=height/2;
}

TextComponent::TextComponent(const TextComponent &other){
    m_TextRenderer=other.m_TextRenderer;
    m_Text=other.m_Text;
    m_Offset=other.m_Offset;
    m_Color=other.m_Color;
    m_Scale=other.m_Scale;
    m_Layer=other.m_Layer;
    m_IgnoreLighting=other.m_IgnoreLighting;
    m_UID=other.m_UID;
}

TextComponent::TextComponent(TextComponent &&other){
    m_TextRenderer=other.m_TextRenderer;
    m_Text=other.m_Text;
    m_Offset=other.m_Offset;
    m_Color=other.m_Color;
    m_Scale=other.m_Scale;
    m_Layer=other.m_Layer;
    m_IgnoreLighting=other.m_IgnoreLighting;
    m_UID=other.m_UID;
}

TextComponent &TextComponent::operator=(const TextComponent &other){
    m_TextRenderer=other.m_TextRenderer;
    m_Text=other.m_Text;
    m_Offset=other.m_Offset;
    m_Color=other.m_Color;
    m_Scale=other.m_Scale;
    m_Layer=other.m_Layer;
    m_IgnoreLighting=other.m_IgnoreLighting;
    m_UID=other.m_UID;
    return *this;
}

TextComponent &TextComponent::operator=(TextComponent &&other){
    m_TextRenderer=other.m_TextRenderer;
    m_Text=other.m_Text;
    m_Offset=other.m_Offset;
    m_Color=other.m_Color;
    m_Scale=other.m_Scale;
    m_Layer=other.m_Layer;
    m_IgnoreLighting=other.m_IgnoreLighting;
    m_UID=other.m_UID;
    return *this;
}

void TextComponent::SetCentered(float width,float height){
    auto [t_width,t_height]=m_TextRenderer->GetTextSize(m_Text,m_Scale);
    
    m_Offset.x=(width-t_width)/2;
    m_Offset.y=(height-t_height)/2;
}

std::ostream &operator<<(std::ostream &os,const RigidbodyComponent::BodyType &type){
    switch(type){
        case RigidbodyComponent::BodyType::Static:    
            os<<"Static";
            break;
        case RigidbodyComponent::BodyType::Dynamic:   
            os<<"Dynamic";
            break;
        case RigidbodyComponent::BodyType::Kinematic: 
            os<<"Kinematic";
            break;
    }
    return os;
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

    float m_X,m_Y,m_PreviousX,m_PreviousY;
    float InterpolatedX,InterpolatedY;
    Vec2 camera_pos=scene->GetCamera().GetPosition();

    Entity *entity=nullptr;
    for(int i=0;i<m_Components.size();i++){
        if(!m_Components[i].m_Visible){
            continue;
        }

        m_X=scene->GetEntityX(m_Components[i].m_UID);
        m_Y=scene->GetEntityY(m_Components[i].m_UID);
        m_PreviousX=scene->GetEntityPreviousX(m_Components[i].m_UID);
        m_PreviousY=scene->GetEntityPreviousY(m_Components[i].m_UID);

        InterpolatedX=Interpolate(m_X,m_PreviousX);
        InterpolatedY=Interpolate(m_Y,m_PreviousY);

        if(InterpolatedX+m_Components[i].m_Width<camera_pos.x || InterpolatedX>camera_pos.x+Window::MAX_WIDTH ||
            InterpolatedY+m_Components[i].m_Height<camera_pos.y || InterpolatedY>camera_pos.y+Window::MAX_HEIGHT){
            continue;
        }

        if(m_Components[i].m_Texture.get()->GetTexID()!=std::numeric_limits<uint32_t>::max()){
            RENDERER->DrawTexture({InterpolatedX,InterpolatedY},{m_Components[i].m_Width,m_Components[i].m_Height},m_Components[i].m_FlipX,m_Components[i].m_FlipY,m_Components[i].m_Layer,*m_Components[i].m_Texture);
        }
    }
}

template<>
void ComponentManager<AnimatedTextureComponent>::Render(Scene *scene){
    PROFILE_FUNCTION();

    float m_X,m_Y,m_PreviousX,m_PreviousY;
    float InterpolatedX,InterpolatedY;
    Vec2 camera_pos=scene->GetCamera().GetPosition();

    Entity *entity=nullptr;
    for(int i=0;i<m_Components.size();i++){
        if(!m_Components[i].m_Visible){
            continue;
        }

        m_X=scene->GetEntityX(m_Components[i].m_UID);
        m_Y=scene->GetEntityY(m_Components[i].m_UID);
        m_PreviousX=scene->GetEntityPreviousX(m_Components[i].m_UID);
        m_PreviousY=scene->GetEntityPreviousY(m_Components[i].m_UID);

        InterpolatedX=Interpolate(m_X,m_PreviousX);
        InterpolatedY=Interpolate(m_Y,m_PreviousY);

        if(InterpolatedX+m_Components[i].m_Width<camera_pos.x || InterpolatedX>camera_pos.x+Window::MAX_WIDTH ||
            InterpolatedY+m_Components[i].m_Height<camera_pos.y || InterpolatedY>camera_pos.y+Window::MAX_HEIGHT){
            continue;
        }

        if(m_Components[i].m_AnimatedTexture.get()->GetTexID()!=std::numeric_limits<uint32_t>::max()){
            RENDERER->DrawAnimatedTexture({InterpolatedX,InterpolatedY},{m_Components[i].m_Width,m_Components[i].m_Height},m_Components[i].m_Layer,m_Components[i].m_FlipX,m_Components[i].m_FlipY,*m_Components[i].m_AnimatedTexture,
                m_Components[i].m_PlayAnimation,m_Components[i].m_LoopAnimation,m_Components[i].m_AnimationDelay,m_Components[i].m_LastAnimationTime,m_Components[i].m_AnimationRow,m_Components[i].m_AnimationIndex);  
        }
    }
}

template<>
void ComponentManager<LightComponent>::Render(Scene *scene){
    PROFILE_FUNCTION();

    float m_X,m_Y,m_PreviousX,m_PreviousY; 
    float InterpolatedX,InterpolatedY;
    float ClosestX,ClosestY;
    float DistanceX,DistanceY;
    float DistanceSquared;
    Vec2 camera_pos=scene->GetCamera().GetPosition();

    Entity *entity=nullptr;
    for(int i=0;i<m_Components.size();i++){
        m_X=scene->GetEntityX(m_Components[i].m_UID);
        m_Y=scene->GetEntityY(m_Components[i].m_UID);
        m_PreviousX=scene->GetEntityPreviousX(m_Components[i].m_UID);
        m_PreviousY=scene->GetEntityPreviousY(m_Components[i].m_UID);

        InterpolatedX=Interpolate(m_X,m_PreviousX)+m_Components[i].m_XOffset;
        InterpolatedY=Interpolate(m_Y,m_PreviousY)+m_Components[i].m_YOffset;

        ClosestX=glm::clamp(InterpolatedX,camera_pos.x,camera_pos.x+Window::MAX_WIDTH);
        ClosestY=glm::clamp(InterpolatedY,camera_pos.y,camera_pos.y+Window::MAX_HEIGHT);
        
        DistanceX=InterpolatedX-ClosestX;
        DistanceY=InterpolatedY-ClosestY;

        DistanceSquared=(DistanceX*DistanceX)+(DistanceY*DistanceY);

        if(DistanceSquared<=m_Components[i].m_Radius*m_Components[i].m_Radius){
            RENDERER->DrawLight({InterpolatedX,InterpolatedY},Vec4(m_Components[i].m_Color.r,m_Components[i].m_Color.g,m_Components[i].m_Color.b,1.0f),m_Components[i].m_Type,m_Components[i].m_Radius,m_Components[i].m_Blur);
        }
    }
}

template<>
void ComponentManager<TextComponent>::Render(Scene *scene){
    PROFILE_FUNCTION();

    float m_X,m_Y,m_PreviousX,m_PreviousY;
    float InterpolatedX,InterpolatedY;
    std::pair<float,float> size;
    Vec2 camera_pos=scene->GetCamera().GetPosition();

    Entity *entity=nullptr;
    for(int i=0;i<m_Components.size();i++){
        m_X=scene->GetEntityX(m_Components[i].m_UID);
        m_Y=scene->GetEntityY(m_Components[i].m_UID);
        m_PreviousX=scene->GetEntityPreviousX(m_Components[i].m_UID);
        m_PreviousY=scene->GetEntityPreviousY(m_Components[i].m_UID);

        InterpolatedX=Interpolate(m_X,m_PreviousX)+m_Components[i].m_Offset.x;
        InterpolatedY=Interpolate(m_Y,m_PreviousY)+m_Components[i].m_Offset.y;

        if(m_Components[i].m_TextRenderer==nullptr){
            continue;
        }

        size=m_Components[i].m_TextRenderer->GetTextSize(m_Components[i].m_Text,m_Components[i].m_Scale);

        if((InterpolatedX+size.first<camera_pos.x || InterpolatedX>camera_pos.x+Window::MAX_WIDTH ||
            InterpolatedY+size.second<camera_pos.y || InterpolatedY>camera_pos.y+Window::MAX_HEIGHT)
            && !m_Components[i].m_TextRenderer->m_Fixed){
            continue;
        }

        m_Components[i].m_TextRenderer->DrawText(m_Components[i].m_Text,InterpolatedX,InterpolatedY,m_Components[i].m_Scale,(m_Components[i].m_IgnoreLighting?std::numeric_limits<int>::max()-1:m_Components[i].m_Layer),m_Components[i].m_Color);
    }
}