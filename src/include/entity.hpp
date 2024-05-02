#pragma once

#include <structs.hpp>
#include <texture.hpp>
#include <window.hpp>
#include <textrenderer.hpp>

extern uint32_t NEXT_UID; //the next uid to be assigned

class Entity;
class TagComponent;
class TextureComponent;
class AnimatedTextureComponent;
class RigidbodyComponent;
class BoxColliderComponent;
class CircleColliderComponent;
class LightComponent;
class NativeScriptComponent;
class TextComponent;
class Scene;

template<typename T>
using ComponentType=typename std::enable_if<
    std::is_same<T,TagComponent>::value ||
    std::is_same<T,TextureComponent>::value ||
    std::is_same<T,AnimatedTextureComponent>::value ||
    std::is_same<T,RigidbodyComponent>::value ||
    std::is_same<T,BoxColliderComponent>::value ||
    std::is_same<T,CircleColliderComponent>::value ||
    std::is_same<T,LightComponent>::value ||
    std::is_same<T,NativeScriptComponent>::value ||
    std::is_same<T,TextComponent>::value,
    int>::type;

/**
 * Search for the component with the given uid.
 * \return the index of the component with the given uid, or -1 if not found
*/
template<typename T,ComponentType<T> = 0>
inline int BinarySearch(std::vector<T> &v,uint32_t uid){
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

/**
 * Search for the first component with uid greater than the given uid.
*/
template<typename T,ComponentType<T> = 0>
inline int FirstGreaterThan(std::vector<T> &v,uint32_t uid){
    int l=0,r=v.size()-1;
    while(l<=r){
        int m=(l+r)/2;
        if(v[m].m_UID>uid)
            r=m-1;
        else
            l=m+1;
    }
    return l;
}

/**
 * Shift the elements of the vector to the right starting from the given index.
 * \param idx the index from which to start shifting
*/
template<typename T,ComponentType<T> = 0>
inline void RightShift(std::vector<T> &v,size_t idx){
    for(size_t i=v.size()-1;i>idx;i--)
        v[i]=v[i-1];
}

/**
 * Interpolate between the current and previous value using Window::Alpha
*/
inline float Interpolate(float current,float previous){
    return current*Window::Alpha+previous*(1.0f-Window::Alpha);
}

class TagComponent{
public:
    TagComponent(const std::string &tag,uint32_t uid): m_Tag(tag),m_UID(uid){
        m_Tag.resize(64);
    }
    TagComponent(): m_UID(std::numeric_limits<uint32_t>::max()){
        m_Tag.resize(64);
    }
    TagComponent(TagComponent &other);
    TagComponent(TagComponent &&other);

    TagComponent &operator=(TagComponent &b){
        m_Tag=b.m_Tag;
        m_UID=b.m_UID;
        return *this;
    }
    TagComponent &operator=(TagComponent &&b){
        if(this!=&b){
            m_Tag=std::move(b.m_Tag);
            m_UID=b.m_UID;
        }
        return *this;
    }

    std::string m_Tag;
    uint32_t m_UID;
};

class TextureComponent{
public:
    TextureComponent(const std::string &path,int mag_filter,int min_filter,float width,float height,int layer,bool visible,bool flipx,bool flipy,uint32_t uid);
    TextureComponent(std::shared_ptr<Texture>t,float width,float height,int layer,bool visible,bool flipx,bool flipy,uint32_t uid);
    TextureComponent(): m_Texture(std::make_shared<Texture>()),m_Width(0),m_Height(0),m_Layer(0),m_Visible(false),m_FlipX(false),m_FlipY(false),m_UID(std::numeric_limits<uint32_t>::max()){}
    TextureComponent(uint32_t uid): m_Texture(std::make_shared<Texture>()),m_Width(0),m_Height(0),m_Layer(0),m_Visible(false),m_FlipX(false),m_FlipY(false),m_UID(uid){}
    TextureComponent(TextureComponent &other);
    TextureComponent(TextureComponent &&other);
    
    TextureComponent &operator=(TextureComponent &b){
        m_Texture=b.m_Texture;
        m_Width=b.m_Width;
        m_Height=b.m_Height;
        m_Layer=b.m_Layer;
        m_Visible=b.m_Visible;
        m_FlipX=b.m_FlipX;
        m_FlipY=b.m_FlipY;
        m_UID=b.m_UID;
        return *this;
    }
    TextureComponent &operator=(TextureComponent &&b){
        if(this!=&b){
            m_Texture=std::move(b.m_Texture);
            m_Width=b.m_Width;
            m_Height=b.m_Height;
            m_Layer=b.m_Layer;
            m_Visible=b.m_Visible;
            m_FlipX=b.m_FlipX;
            m_FlipY=b.m_FlipY;
            m_UID=b.m_UID;
        }
        return *this;
    }

    std::shared_ptr<Texture> m_Texture;
    float m_Width,m_Height;
    int m_Layer;
    bool m_Visible;
    bool m_FlipX;
    bool m_FlipY;
    uint32_t m_UID;
};

class AnimatedTextureComponent{
public:
    AnimatedTextureComponent(const std::string &path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter,float width,float height,int layer,bool play_animation,bool loop_animation,float animation_delay,int animation_row,int animation_index,bool visible,bool flipx,bool flipy,uint32_t uid);
    AnimatedTextureComponent(std::shared_ptr<SpriteSheet>t,float width,float height,int layer,bool play_animation,bool loop_animation,float animation_delay,int animation_row,int animation_index,bool visible,bool flipx,bool flipy,uint32_t uid);
    AnimatedTextureComponent(): m_AnimatedTexture(std::make_shared<SpriteSheet>()),m_Width(0),m_Height(0),m_Layer(0),m_PlayAnimation(false),m_LoopAnimation(false),m_AnimationDelay(0),m_AnimationRow(0),m_AnimationIndex(0),m_LastAnimationTime(0),m_Visible(false),m_FlipX(false),m_FlipY(false),m_UID(std::numeric_limits<uint32_t>::max()){}
    AnimatedTextureComponent(uint32_t uid): m_AnimatedTexture(std::make_shared<SpriteSheet>()),m_Width(0),m_Height(0),m_Layer(0),m_PlayAnimation(false),m_LoopAnimation(false),m_AnimationDelay(0),m_AnimationRow(0),m_AnimationIndex(0),m_LastAnimationTime(0),m_Visible(false),m_FlipX(false),m_FlipY(false),m_UID(uid){}
    AnimatedTextureComponent(AnimatedTextureComponent &other);
    AnimatedTextureComponent(AnimatedTextureComponent &&other);

    AnimatedTextureComponent &operator=(AnimatedTextureComponent &b){
        m_AnimatedTexture=b.m_AnimatedTexture;
        m_Width=b.m_Width;
        m_Height=b.m_Height;
        m_Layer=b.m_Layer;
        m_PlayAnimation=b.m_PlayAnimation;
        m_LoopAnimation=b.m_LoopAnimation;
        m_AnimationDelay=b.m_AnimationDelay;
        m_AnimationRow=b.m_AnimationRow;
        m_AnimationIndex=b.m_AnimationIndex;
        m_LastAnimationTime=b.m_LastAnimationTime;
        m_Visible=b.m_Visible;
        m_FlipX=b.m_FlipX;
        m_FlipY=b.m_FlipY;
        m_UID=b.m_UID;
        return *this;
    }
    AnimatedTextureComponent &operator=(AnimatedTextureComponent &&b){
        if(this!=&b){
            m_AnimatedTexture=std::move(b.m_AnimatedTexture);
            m_Width=b.m_Width;
            m_Height=b.m_Height;
            m_Layer=b.m_Layer;
            m_PlayAnimation=b.m_PlayAnimation;
            m_LoopAnimation=b.m_LoopAnimation;
            m_AnimationDelay=b.m_AnimationDelay;
            m_AnimationRow=b.m_AnimationRow;
            m_AnimationIndex=b.m_AnimationIndex;
            m_LastAnimationTime=b.m_LastAnimationTime;
            m_Visible=b.m_Visible;
            m_FlipX=b.m_FlipX;
            m_FlipY=b.m_FlipY;
            m_UID=b.m_UID;
        }
        return *this;
    }

    void PlayAnimation(bool loop=false,float delay=0.0f);

    std::shared_ptr<SpriteSheet> m_AnimatedTexture;
    float m_Width,m_Height;
    int m_Layer;
    bool m_PlayAnimation,m_LoopAnimation;
    float m_AnimationDelay;
    int m_AnimationIndex;
    int m_AnimationRow;
    float m_LastAnimationTime;
    bool m_Visible;
    bool m_FlipX;
    bool m_FlipY;
    uint32_t m_UID;
};

class RigidbodyComponent{
public:
    enum class BodyType{
        Static,
        Dynamic,
        Kinematic
    };

    RigidbodyComponent(){ m_UID=std::numeric_limits<uint32_t>::max(); }
    RigidbodyComponent(uint32_t uid): m_UID(uid){}
    RigidbodyComponent(BodyType bodyType,bool fixedRotation,uint32_t uid): m_BodyType(bodyType),m_FixedRotation(fixedRotation),m_UID(uid){}

    BodyType m_BodyType=BodyType::Static;
    bool m_FixedRotation=true;
    b2Body *m_RuntimeBody=nullptr;

    uint32_t m_UID;
};

std::ostream &operator<<(std::ostream &os,const RigidbodyComponent::BodyType &type);

class BoxColliderComponent{
public:
    BoxColliderComponent(){ m_UID=std::numeric_limits<uint32_t>::max(); }
    BoxColliderComponent(uint32_t uid): m_UID(uid){}
    BoxColliderComponent(float x_offset,float y_offset,float width,float height,float density,float friction,
    float restitution,float restitution_threshold,uint16_t category_bits,uint16_t mask_bits,bool is_sensor,uint32_t uid): m_XOffset(x_offset),m_YOffset(y_offset),m_Width(width),m_Height(height),
    m_Density(density),m_Friction(friction),m_Restitution(restitution),m_RestitutionThreshold(restitution_threshold),m_CategoryBits(category_bits),m_MaskBits(mask_bits),m_IsSensor(is_sensor),m_UID(uid){}

    float m_XOffset=0.0f,m_YOffset=0.0f;
    float m_Width=0.0f,m_Height=0.0f;

    float m_Density=1.0f;
    float m_Friction=0.0f;
    float m_Restitution=0.0f;
    float m_RestitutionThreshold=0.5f;
    uint16_t m_CategoryBits=1;
    uint16_t m_MaskBits=0xFFFF;
    bool m_IsSensor=false;

    b2Fixture *m_RuntimeFixture=nullptr;

    uint32_t m_UID;
};

class CircleColliderComponent{
public:
    CircleColliderComponent(){ m_UID=std::numeric_limits<uint32_t>::max(); } 
    CircleColliderComponent(uint32_t uid): m_UID(uid){}
    CircleColliderComponent(float x_offset,float y_offset,float radius,float density,float friction,
    float restitution,float restitution_threshold,uint16_t category_bits,uint16_t mask_bits,bool is_sensor,uint32_t uid): m_XOffset(x_offset),m_YOffset(y_offset),m_Radius(radius),
    m_Density(density),m_Friction(friction),m_Restitution(restitution),m_RestitutionThreshold(restitution_threshold),m_CategoryBits(category_bits),m_MaskBits(mask_bits),m_IsSensor(is_sensor),m_UID(uid){}

    float m_XOffset=0.0f,m_YOffset=0.0f;
    float m_Radius=0.0f;

    float m_Density=1.0f;
    float m_Friction=0.0f;
    float m_Restitution=0.0f;
    float m_RestitutionThreshold=0.5f;
    uint16_t m_CategoryBits=1;
    uint16_t m_MaskBits=0xFFFF;
    bool m_IsSensor=false;

    b2Fixture *m_RuntimeFixture=nullptr;

    uint32_t m_UID;
};

class LightComponent{
public:
    LightComponent();
    LightComponent(float x_offset,float y_offset,float radius,float blur,Vec3 color,LightType type,uint32_t uid);
    LightComponent(uint32_t uid);

    void SetOffset(float x_offset,float y_offset);
    void SetCentered(float width,float height);

    float m_XOffset,m_YOffset;
    float m_Radius,m_Blur;
    Vec3 m_Color;
    LightType m_Type;

    uint32_t m_UID;
};

class NativeScriptComponent{
public:
    NativeScriptComponent(): m_UID(std::numeric_limits<uint32_t>::max()){}
    NativeScriptComponent(uint32_t uid): m_UID(uid){}

    std::function<void(Scene*,NativeScriptComponent*)> OnCreate;
    std::function<void(Scene*,NativeScriptComponent*,float)> OnUpdate;
    std::function<void(Scene*,NativeScriptComponent*)> OnDestroy;

    uint32_t m_UID;
};

class TextComponent{
public:
    TextComponent(): m_TextRenderer(std::make_shared<TextRenderer>()),m_Text(100,'\0'),m_Offset(0.0f,0.0f),m_Color(1.0f,1.0f,1.0f),m_Scale(1.0f),m_Layer(0),m_IgnoreLighting(false),m_UID(std::numeric_limits<uint32_t>::max()){}
    TextComponent(uint32_t uid): m_TextRenderer(std::make_shared<TextRenderer>()),m_Text(100,'\0'),m_Offset(0.0f,0.0f),m_Color(1.0f,1.0f,1.0f),m_Scale(1.0f),m_Layer(0),m_IgnoreLighting(false),m_UID(uid){}
    TextComponent(std::shared_ptr<TextRenderer> textRenderer,const std::string &text,Vec2 offset,Vec3 color,float scale,uint32_t uid): 
        m_TextRenderer(textRenderer),m_Text(text),m_Offset(offset),m_Color(color),m_Scale(scale),m_Layer(0),m_IgnoreLighting(false),m_UID(uid){
            m_Text.resize(100);
        }
    TextComponent(const std::string &path,float glyph_size,bool fixed,uint32_t uid): m_TextRenderer(FONT_MANAGER->GetFont(path,glyph_size,fixed).second),m_Layer(0),m_IgnoreLighting(false),m_UID(uid){}
    TextComponent(const std::string &path,float glyph_size,bool fixed,const std::string &text,Vec2 offset,Vec3 color,float scale,int layer,bool ignore_lighting,uint32_t uid): 
        m_TextRenderer(FONT_MANAGER->GetFont(path,glyph_size,fixed).second),m_Text(text),m_Offset(offset),m_Color(color),m_Scale(scale),m_Layer(layer),m_IgnoreLighting(ignore_lighting),m_UID(uid){
            m_Text.resize(100);
        }

    TextComponent(const TextComponent &other);
    TextComponent(TextComponent &&other);
    TextComponent &operator=(const TextComponent &other);
    TextComponent &operator=(TextComponent &&other);

    void SetCentered(float width,float height);

    std::shared_ptr<TextRenderer> m_TextRenderer;
    std::string m_Text;
    Vec2 m_Offset;
    Vec3 m_Color;
    float m_Scale;
    int m_Layer;
    bool m_IgnoreLighting;
    uint32_t m_UID;
};

class Entity{
public:
    Entity(uint32_t parent=std::numeric_limits<uint32_t>::max());
    Entity(float x,float y);
    Entity(uint32_t uid,uint32_t parent,uint32_t group);
    Entity(const Entity &other);
    Entity(Entity &&other);
    Entity &operator=(const Entity &other);
    Entity &operator=(Entity &&other);

    uint32_t m_UID;
    uint32_t m_Parent;
    uint32_t m_Group;
    float m_X,m_Y;
    float m_PreviousX,m_PreviousY;
};

/**
 * Search for the entity with the given uid.
 * \return a pointer to the entity with the given uid, or nullptr if not found
*/
inline Entity* BinarySearch(std::vector<Entity> &v,uint32_t uid){
    int l=0,r=v.size()-1;
    while(l<=r){
        int m=(l+r)/2;
        if(v[m].m_UID==uid)
            return &v[m];
        else if(v[m].m_UID<uid)
            l=m+1;
        else
            r=m-1;
    }
    return nullptr;
}

template<typename T,ComponentType<T> = 0>
class ComponentManager{
public:
    std::vector<T> m_Components;

    /**
     * Add a component to the entity with the given uid.
    */
    void AddComponent(T &component,uint32_t uid){
        int idx=FirstGreaterThan(m_Components,uid);
        m_Components.resize(m_Components.size()+1);
        RightShift(m_Components,idx);
        m_Components[idx]=component;
    }

    /**
     * Remove the component with the given uid.
    */
    void RemoveComponent(uint32_t uid){
        int idx;
        if((idx=BinarySearch(m_Components,uid))!=-1){
            if constexpr(std::is_same<T,RigidbodyComponent>::value)
                m_Components[idx].m_RuntimeBody->GetWorld()->DestroyBody(m_Components[idx].m_RuntimeBody);
            if constexpr(std::is_same<T,BoxColliderComponent>::value)
                m_Components[idx].m_RuntimeFixture->GetBody()->DestroyFixture(m_Components[idx].m_RuntimeFixture);
            if constexpr(std::is_same<T,CircleColliderComponent>::value)
                m_Components[idx].m_RuntimeFixture->GetBody()->DestroyFixture(m_Components[idx].m_RuntimeFixture);
            m_Components.erase(m_Components.begin()+idx);
        }
    }

    /**
     * \return pointer to the component with the given uid, or nullptr if not found
    */
    T* GetComponent(uint32_t uid){
        int idx;
        if((idx=BinarySearch(m_Components,uid))!=-1)
            return &m_Components[idx];
        else
            return nullptr;
    }

    /**
     * \return a reference to the components vector
    */
    std::vector<T> &GetComponents(){
        return m_Components;
    }

    /**
     * \return a pointer to the component found at the given index, converted to a Rect
    */
    Rect* GetComponentAsRect(int index){
        Rect *r=new Rect;
        r->pos.x=m_Components[index].m_XOffset; //entity position is added later
        r->pos.y=m_Components[index].m_YOffset;
        r->size.x=m_Components[index].m_Width;
        r->size.y=m_Components[index].m_Height;
        r->vel.x=m_Components[index].m_HSpeed;
        r->vel.y=m_Components[index].m_VSpeed;
        return r;
    }

    /**
     * Updates all the components in the vector. Some components type don't need to be updated.
    */
    void Update(Scene *scene,float frame_time);
    /**
     * Renders all the components in the vector. Some components type don't need to be rendered.
    */
    void Render(Scene *scene);
};

/**
 * \return the body type converted to a b2BodyType
*/
inline b2BodyType RigidbodyTypeToBox2DBody(RigidbodyComponent::BodyType bodyType){
    switch (bodyType){
        case RigidbodyComponent::BodyType::Static:    
            return b2_staticBody;
        case RigidbodyComponent::BodyType::Dynamic:   
            return b2_dynamicBody;
        case RigidbodyComponent::BodyType::Kinematic: 
            return b2_kinematicBody;
    }
    return b2_staticBody;
}

/**
 * \return the b2BodyType converted to a BodyType
*/
inline RigidbodyComponent::BodyType RigidbodyTypeFromBox2DBody(b2BodyType bodyType){
    switch (bodyType){
        case b2_staticBody:    
            return RigidbodyComponent::BodyType::Static;   
        case b2_dynamicBody:   
            return RigidbodyComponent::BodyType::Dynamic;  
        case b2_kinematicBody: 
            return RigidbodyComponent::BodyType::Kinematic;
    }
    printf("Unknown body type\n");
    return RigidbodyComponent::BodyType::Static;
}