#include <entity.hpp>
#include <algorithm>

Entity::Entity(const char *path,float x,float y,float w,float h,bool pixelart): m_R(),m_Depth(0),m_T(path,(pixelart)?GL_NEAREST:GL_LINEAR,(pixelart)?GL_NEAREST:GL_LINEAR),m_Collider(false){
    m_R.pos={x,y};
    m_R.size={w,h};
}

void Entity::SetSize(int w,int h){
    m_R.pos.w=w;
    m_R.pos.h=h;
}

void Entity::SetTexture(const char *path,bool pixelart){
    (pixelart)?m_T=Texture(path,GL_NEAREST,GL_NEAREST):m_T=Texture(path,GL_LINEAR,GL_LINEAR);
}

void Entity::Draw(Renderer &renderer){
    renderer.DrawTexture(m_R.pos.x,m_R.pos.y,m_R.size.w,m_R.size.h,1.0f,m_Depth,m_T.GetTexID());
}

void Entity::Move(int x_offset,int y_offset,float frame_time){
    m_R.vel.x+=x_offset;
    m_R.vel.y+=y_offset;
    if(m_Collider){
        Vec2 cp,cn;
        float m_T=0;
        std::vector<std::pair<int,float>>z;

        for(size_t i=0;i<CollisionDetection::COLLIDERS.size();i++){
            if(CollisionDetection::DynamicRectVsRect(&m_R,frame_time,CollisionDetection::COLLIDERS[i],cp,cn,m_T))
                z.push_back({i,m_T});
        }

        std::sort(begin(z),end(z),[](const std::pair<int,float> &a,const std::pair<int,float> &b){
            return a.second<b.second;
        });

        for(auto j:z)
			CollisionDetection::ResolveDynamicRectVsRect(&m_R,frame_time,&CollisionDetection::COLLIDERS[j.first]);
    }
    m_R.pos=m_R.pos+m_R.vel*frame_time;
    m_R.vel={0,0};
}

void Entity::AddCollider(bool insert){
    m_Collider=true;
    if(insert)
        CollisionDetection::COLLIDERS.push_back(m_R);
}

//ORIGINAL CODE: https://github.com/OneLoneCoder/Javidx9/blob/master/PixelGameEngine/SmallerProjects/OneLoneCoder_PGE_Rectangles.cpp
namespace CollisionDetection{
    bool RayVsRect(const Vec2 &ray_origin, const Vec2 &ray_dir,const Rect *target,Vec2 &contact_point,Vec2 &contact_normal,float& t_hit_near){
        contact_normal={0,0};
        contact_point={0,0};

        // Cache division
        Vec2 invdir=1.0f/ray_dir;

        // Calculate intersections with rectangle bounding axes
        Vec2 t_near=(target->pos-ray_origin)*invdir;
        Vec2 t_far=(target->pos+target->size-ray_origin)*invdir;

        // Check division by 0
        if(std::isnan(t_far.y) || std::isnan(t_far.x)) return false;
        if(std::isnan(t_near.y) || std::isnan(t_near.x)) return false;

        // Sort distances
        if(t_near.x>t_far.x) std::swap(t_near.x,t_far.x);
        if(t_near.y>t_far.y) std::swap(t_near.y,t_far.y);

        // Early rejection		
        if(t_near.x>t_far.y || t_near.y>t_far.x) return false;

        // Closest 'time' will be the first contact
        t_hit_near=std::max(t_near.x,t_near.y);

        // Furthest 'time' is contact on opposite side of target
        float t_hit_far=std::min(t_far.x,t_far.y);

        // Reject if ray direction is pointing away from object
        if(t_hit_far<0)
            return false;

        // Contact point of collision from parametric line equation
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
        // Note if t_near == t_far, collision is principly in a diagonal
        // so pointless to resolve. By returning a CN={0,0} even though its
        // considered a hit, the resolver wont change anything.
        return true;
    }

    bool DynamicRectVsRect(const Rect* r_dynamic,const float fTimeStep,const Rect& r_static,
        Vec2 &contact_point, Vec2 &contact_normal, float& contact_time){
        // Check if dynamic rectangle is actually moving - we assume rectangles are NOT in collision to start
        if(r_dynamic->vel.x==0 && r_dynamic->vel.y==0)
            return false;

        // Expand target rectangle by source dimensions
        Rect expanded_target;
        expanded_target.pos=r_static.pos-r_dynamic->size/2;
        expanded_target.size=r_static.size+r_dynamic->size;

        if(RayVsRect(r_dynamic->pos+r_dynamic->size/2,r_dynamic->vel*fTimeStep,&expanded_target,contact_point,contact_normal,contact_time))
            return (contact_time>=0.0f && contact_time<1.0f);
        else
            return false;
    }

    bool ResolveDynamicRectVsRect(Rect* r_dynamic,const float fTimeStep,Rect* r_static){
        Vec2 contact_point,contact_normal;
        float contact_time=0.0f;
        if(DynamicRectVsRect(r_dynamic,fTimeStep,*r_static,contact_point,contact_normal,contact_time)){
            r_dynamic->vel=r_dynamic->vel+contact_normal*Vec2(std::abs(r_dynamic->vel.x),std::abs(r_dynamic->vel.y))*(1-contact_time);
            return true;
        }

        return false;
    }

    std::vector<Rect>COLLIDERS;
}