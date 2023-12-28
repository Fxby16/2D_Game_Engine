#pragma once

#include <structs.hpp>
#include <texture.hpp>
#include <vector>

class Entity;
class TextureComponent;
class AnimatedTextureComponent;
class ColliderComponent;
class LightComponent;

template<typename T>
using ComponentType=typename std::enable_if<
    std::is_same<T,TextureComponent>::value ||
    std::is_same<T,AnimatedTextureComponent>::value ||
    std::is_same<T,ColliderComponent>::value ||
    std::is_same<T,LightComponent>::value,
    int>::type;

template<typename T,ComponentType<T> = 0>
inline int BinarySearch(std::vector<T> &v,uint64_t uid){
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

template<typename T,ComponentType<T> = 0>
inline int FirstGreaterThan(std::vector<T> &v,uint64_t uid){
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

template<typename T,ComponentType<T> = 0>
inline void RightShift(std::vector<T> &v,size_t idx){
    for(size_t i=v.size()-1;i>idx;i--)
        v[i]=v[i-1];
}

class TextureComponent{
public:
    TextureComponent(const std::string &path,int mag_filter,int min_filter,float width,float height,float layer,uint64_t uid);
    TextureComponent(): m_Texture(),m_Width(0),m_Height(0),m_Layer(0),m_UID(std::numeric_limits<uint64_t>::max()){}
    TextureComponent(TextureComponent &other);
    TextureComponent(TextureComponent &&other);
    
    TextureComponent &operator=(TextureComponent &b){
        m_Texture=b.m_Texture;
        m_Width=b.m_Width;
        m_Height=b.m_Height;
        m_Layer=b.m_Layer;
        m_UID=b.m_UID;
        return *this;
    }
    TextureComponent &operator=(TextureComponent &&b){
        if(this!=&b){
            m_Texture=std::move(b.m_Texture);
            m_Width=b.m_Width;
            m_Height=b.m_Height;
            m_Layer=b.m_Layer;
            m_UID=b.m_UID;
        }
        return *this;
    }

    Texture m_Texture;
    float m_Width,m_Height;
    float m_Layer;
    uint64_t m_UID;
};

class AnimatedTextureComponent{
public:
    AnimatedTextureComponent(const std::string &path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter,float width,float height,float layer,uint64_t uid);
    AnimatedTextureComponent(): m_AnimatedTexture(),m_Width(0),m_Height(0),m_Layer(0),m_UID(std::numeric_limits<uint64_t>::max()){}
    AnimatedTextureComponent(AnimatedTextureComponent &other);
    AnimatedTextureComponent(AnimatedTextureComponent &&other);

    AnimatedTextureComponent &operator=(AnimatedTextureComponent &b){
        m_AnimatedTexture=b.m_AnimatedTexture;
        m_Width=b.m_Width;
        m_Height=b.m_Height;
        m_Layer=b.m_Layer;
        m_UID=b.m_UID;
        return *this;
    }
    AnimatedTextureComponent &operator=(AnimatedTextureComponent &&b){
        if(this!=&b){
            m_AnimatedTexture=std::move(b.m_AnimatedTexture);
            m_Width=b.m_Width;
            m_Height=b.m_Height;
            m_Layer=b.m_Layer;
            m_UID=b.m_UID;
        }
        return *this;
    }

    void PlayAnimation(bool loop=false,float delay=0.0f);

    AnimatedTexture m_AnimatedTexture;
    float m_Width,m_Height;
    float m_Layer;
    uint64_t m_UID;
};

class ColliderComponent{
public:
    ColliderComponent(float width,float height,float hspeed,float vspeed,float xoffset,float yoffset,uint64_t uid);
    ColliderComponent(): m_Width(0),m_Height(0),m_HSpeed(0),m_VSpeed(0),m_XOffset(0),m_YOffset(0),m_UID(std::numeric_limits<uint64_t>::max()){}

    static bool RayVsRect(const Vec2 &ray_origin, const Vec2 &ray_dir,const Rect *target,Vec2 &contact_point,Vec2 &contact_normal,float &time_hit_near);
    static bool DynamicRectVsRect(const Rect *dynamic_rect,const float frame_time,const Rect *static_rect,
        Vec2 &contact_point,Vec2 &contact_normal,float &contact_time);
    static bool ResolveDynamicRectVsRect(Rect *dynamic_rect,const float frame_time,Rect *static_rect);

    void Move(float x_offset,float y_offset);

    float m_Width,m_Height;
    float m_HSpeed,m_VSpeed;
    float m_XOffset,m_YOffset;

    uint64_t m_UID;
};

class LightComponent{
public:
    LightComponent();
    LightComponent(float x_offset,float y_offset,float radius,float blur,Vec3 color,LightType type,uint64_t uid);

    void SetOffset(float x_offset,float y_offset);
    void SetCentered(float width,float height);

    float m_XOffset,m_YOffset;
    float m_Radius,m_Blur;
    Vec3 m_Color;
    LightType m_Type;

    uint64_t m_UID;
};

class Entity{
public:
    Entity();
    Entity(float x,float y);
    Entity(Entity &other);
    Entity(Entity &&other);
    Entity &operator=(Entity &other);
    Entity &operator=(Entity &&other);

    uint64_t m_UID;
    float m_X,m_Y;
    bool m_HasCollider;
};

inline Entity* BinarySearch(std::vector<Entity> &v,uint64_t uid){
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

    void AddComponent(T &component,uint64_t uid){
        int idx=FirstGreaterThan(m_Components,uid);
        m_Components.resize(m_Components.size()+1);
        RightShift(m_Components,idx);
        m_Components[idx]=component;
    }

    void RemoveComponent(uint64_t uid){
        int idx;
        if((idx=BinarySearch(m_Components,uid))!=-1){
            m_Components.erase(m_Components.begin()+idx);
        }
    }

    /**
     * \return pointer to the component with the given uid, or nullptr if not found
    */
    T* GetComponent(uint64_t uid){
        int idx;
        if((idx=BinarySearch(m_Components,uid))!=-1)
            return &m_Components[idx];
        else
            return nullptr;
    }

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

    void Update(float frame_time,std::vector<Entity>& entities);
    void Render(std::vector<Entity>& entities);
};