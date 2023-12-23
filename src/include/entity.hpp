#pragma once

#include <structs.hpp>
#include <texture.hpp>
#include <vector>

class Entity;
class TextureComponent;
class AnimatedTextureComponent;
class ColliderComponent;

template<typename T>
using ComponentType=typename std::enable_if<
    std::is_same<T,TextureComponent>::value ||
    std::is_same<T,AnimatedTextureComponent>::value ||
    std::is_same<T,ColliderComponent>::value,
    int>::type;

template<typename T,ComponentType<T> = 0>
int BinarySearch(std::vector<T> &v,uint64_t uid){
    int l=0,r=v.size()-1;
    while(l<=r){
        int m=(l+r)/2;
        if(v[m].m_Entity->m_UID==uid)
            return m;
        else if(v[m].m_Entity->m_UID<uid)
            l=m+1;
        else
            r=m-1;
    }
    return -1;
}

template<typename T,ComponentType<T> = 0>
int FirstGreaterThan(std::vector<T> &v,uint64_t uid){
    int l=0,r=v.size()-1;
    while(l<=r){
        int m=(l+r)/2;
        if(v[m].m_Entity->m_UID>uid)
            r=m-1;
        else
            l=m+1;
    }
    return l;
}

template<typename T,ComponentType<T> = 0>
void RightShift(std::vector<T> &v,size_t idx){
    for(size_t i=v.size()-1;i>idx;i--)
        v[i]=v[i-1];
}

class TextureComponent{
public:
    TextureComponent(const std::string &path,int mag_filter,int min_filter,float width,float height,int depth,Entity *entity);
    TextureComponent(): m_Texture(),m_Width(0),m_Height(0),m_Depth(0),m_Entity(nullptr){}
    TextureComponent(TextureComponent &other);
    TextureComponent(TextureComponent &&other);
    
    TextureComponent &operator=(TextureComponent &b){
        m_Texture=b.m_Texture;
        m_Width=b.m_Width;
        m_Height=b.m_Height;
        m_Depth=b.m_Depth;
        m_Entity=b.m_Entity;
        return *this;
    }
    TextureComponent &operator=(TextureComponent &&b){
        if(this!=&b){
            m_Texture=std::move(b.m_Texture);
            m_Width=b.m_Width;
            m_Height=b.m_Height;
            m_Depth=b.m_Depth;
            m_Entity=b.m_Entity;
        }
        return *this;
    }

    Texture m_Texture;
    float m_Width,m_Height;
    int m_Depth;
    Entity *m_Entity;
};

class AnimatedTextureComponent{
public:
    AnimatedTextureComponent(const std::string &path,unsigned int tile_width,unsigned int tile_height,int mag_filter,int min_filter,float width,float height,int depth,Entity *entity);
    AnimatedTextureComponent(): m_AnimatedTexture(),m_Width(0),m_Height(0),m_Depth(0),m_Entity(nullptr){}
    AnimatedTextureComponent(AnimatedTextureComponent &other);
    AnimatedTextureComponent(AnimatedTextureComponent &&other);

    AnimatedTextureComponent &operator=(AnimatedTextureComponent &b){
        m_AnimatedTexture=b.m_AnimatedTexture;
        m_Width=b.m_Width;
        m_Height=b.m_Height;
        m_Depth=b.m_Depth;
        m_Entity=b.m_Entity;
        return *this;
    }
    AnimatedTextureComponent &operator=(AnimatedTextureComponent &&b){
        if(this!=&b){
            m_AnimatedTexture=std::move(b.m_AnimatedTexture);
            m_Width=b.m_Width;
            m_Height=b.m_Height;
            m_Depth=b.m_Depth;
            m_Entity=b.m_Entity;
        }
        return *this;
    }

    AnimatedTexture m_AnimatedTexture;
    float m_Width,m_Height;
    int m_Depth;
    Entity *m_Entity;
};

class ColliderComponent{
public:
    ColliderComponent(float width,float height,float hspeed,float vspeed,float xoffset,float yoffset,Entity *entity);
    ColliderComponent(): m_Width(0),m_Height(0),m_HSpeed(0),m_VSpeed(0),m_XOffset(0),m_YOffset(0),m_Entity(nullptr){}

    static bool RayVsRect(const Vec2 &ray_origin, const Vec2 &ray_dir,const Rect *target,Vec2 &contact_point,Vec2 &contact_normal,float &t_hit_near);
    static bool DynamicRectVsRect(const Rect* r_dynamic,const float frame_time,const Rect* r_static,
        Vec2 &contact_point,Vec2 &contact_normal,float &contact_time);
    static bool ResolveDynamicRectVsRect(Rect* r_dynamic,const float frame_time,Rect *r_static);

    void Move(float x_offset,float y_offset);

    float m_Width,m_Height;
    float m_HSpeed,m_VSpeed;
    float m_XOffset,m_YOffset;

    Entity *m_Entity;
};

class Entity{
public:
    Entity();
    Entity(float x,float y);
    ~Entity();

    void Move(float x_offset,float y_offset);

    template<typename T,typename...Args>
    void AddComponent(Args...args);

    template<typename T>
    void RemoveComponent();

    template<typename T>
    T* GetComponent();
    
    void SetPos(float x,float y);

    uint64_t m_UID;
    float m_X,m_Y;
    bool m_HasCollider;
};

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
            if constexpr(std::is_same<T,ColliderComponent>::value){
                m_Components[idx].m_Entity->m_HasCollider=false;
            }
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
        r->pos.x=m_Components[index].m_Entity->m_X+m_Components[index].m_XOffset;
        r->pos.y=m_Components[index].m_Entity->m_Y+m_Components[index].m_YOffset;
        r->size.x=m_Components[index].m_Width;
        r->size.y=m_Components[index].m_Height;
        r->vel.x=m_Components[index].m_HSpeed;
        r->vel.y=m_Components[index].m_VSpeed;
        return r;
    }

    void Update(float frame_time);
    void Render();
};

extern ComponentManager<TextureComponent> CMTC;
extern ComponentManager<AnimatedTextureComponent> CMATC;
extern ComponentManager<ColliderComponent> CMCC;