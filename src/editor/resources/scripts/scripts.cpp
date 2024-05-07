#include <pch.hpp>
#include <scene.hpp>
#include <buttons.hpp>
#include <window.hpp>
#include <application.hpp>
#include <sceneserializer.hpp>

#define QUESTION_BLOCK_GROUP 1
#define MAP_GROUP 2
#define MARIO_GROUP 3
#define ENEMY_GROUP 4
#define END_MAP_GROUP 5
#define HOLE_SENSOR_GROUP 6
#define UNDER_MAP_GROUP 7

#define MARIO_DEAD 4
#define MARIO_IDLE 3
#define MARIO_RUN 2
#define MARIO_JUMP 1
#define MARIO_STOPPING 0

#define MAP_WIDTH 130.0f

using namespace Window;
Scene *scene=nullptr; 

bool marioIsInAir=false;
std::unordered_map<uint32_t,bool> goombas_direction; //true=right,false=left
std::queue<uint32_t> entities_to_destroy;

class ContactListener : public b2ContactListener{
    void BeginContact(b2Contact *contact) override{
        b2Fixture *fixtureA=contact->GetFixtureA();
        b2Fixture *fixtureB=contact->GetFixtureB();

        uint32_t uid_a=(uint32_t)fixtureA->GetUserData().pointer;
        uint32_t uid_b=(uint32_t)fixtureB->GetUserData().pointer;

        Entity *entity_a=scene->GetEntity(uid_a);
        Entity *entity_b=scene->GetEntity(uid_b);

        if(uid_a->m_Group==UNDER_MAP_GROUP){
            entities_to_destroy.push(uid_a);
            return;
        }

        if(uid_b->m_Group==UNDER_MAP_GROUP){
            entities_to_destroy.push(uid_b);
            return;
        }

        BoxColliderComponent *bcc_a=scene->GetComponent<BoxColliderComponent>(uid_a);
        BoxColliderComponent *bcc_b=scene->GetComponent<BoxColliderComponent>(uid_b);

        //mario collides with map
        if(entity_a->m_Group==MARIO_GROUP){
            if(entity_a->m_Y>=entity_b->m_Y+bcc_b->m_YOffset+bcc_b->m_Height){
                marioIsInAir=false;
            }
        }else if(entity_b->m_Group==MARIO_GROUP){
            if(entity_b->m_Y>=entity_a->m_Y+bcc_a->m_YOffset+bcc_a->m_Height){
                marioIsInAir=false;
            }
        }

        //enemy collides with map
        if(entity_a->m_Group==ENEMY_GROUP && entity_b->m_Group==ENEMY_GROUP){
            goombas_direction[uid_a]=!goombas_direction[uid_a];
            goombas_direction[uid_b]=!goombas_direction[uid_b];
        }else if(entity_a->m_Group==ENEMY_GROUP && entity_b->m_Group!=MARIO_GROUP){
            if(entity_a->m_Y<entity_b->m_Y+bcc_b->m_Height){
                goombas_direction[uid_a]=!goombas_direction[uid_a];
            }
        }else if(entity_b->m_Group==ENEMY_GROUP && entity_a->m_Group!=MARIO_GROUP){
            if(entity_b->m_Y<entity_a->m_Y+bcc_a->m_Height){
                goombas_direction[uid_b]=!goombas_direction[uid_b];
            }
        }

        if(entity_a->m_Group==HOLE_SENSOR_GROUP || entity_b->m_Group==HOLE_SENSOR_GROUP){
            printf("Mario entered the underground\n");
        }
    }

    void EndContact(b2Contact *contact) override{
    }
};

ContactListener contactlistener;

extern SceneSerializer sceneserializer;

void BeforeUpdate(double frame_time,Application *app){
    while(!entities_to_destroy.empty()){
        scene->RemoveEntity(entities_to_destroy.front());
        entities_to_destroy.pop();
    }
}

void AfterUpdate(double frame_time,Application *app){
}

void OnCreate(Application *app){
    scene=app->GetScene();
    scene->GetPhysicsWorld()->SetContactListener(&contactlistener);
}

void OnDestroy(Application *app){
}

void MarioUpdate(Scene *scene,NativeScriptComponent *nsc,float frame_time){
    static bool flip=false; 

    RigidbodyComponent *rb=scene->GetComponent<RigidbodyComponent>(nsc->m_UID);
    AnimatedTextureComponent *atc=scene->GetComponent<AnimatedTextureComponent>(nsc->m_UID);
    Entity *e=scene->GetEntity(nsc->m_UID);

    b2Vec2 vel=rb->m_RuntimeBody->GetLinearVelocity();
    Vec2 desired_vel={0.0f,0.0f};

    KeyState a_key=INPUT->GetKey(KEY_A);
    KeyState d_key=INPUT->GetKey(KEY_D);
    KeyState w_key=INPUT->GetKey(KEY_W);

    if(a_key.current==BUTTON_DOWN && d_key.current!=BUTTON_DOWN){ //mario is moving left
        desired_vel.x=std::max(vel.x-0.1f,-3.0f);
        flip=true;
        atc->m_FlipX=true;
        atc->m_AnimationRow=MARIO_RUN;
        atc->m_PlayAnimation=true;
        atc->m_LoopAnimation=true;
        atc->m_AnimationDelay=0.200f;
    }else if(d_key.current==BUTTON_DOWN && a_key.current!=BUTTON_DOWN){ //mario is moving right
        desired_vel.x=std::min(vel.x+0.1f,3.0f);
        flip=false;
        atc->m_FlipX=false;
        atc->m_AnimationRow=MARIO_RUN;
        atc->m_PlayAnimation=true;
        atc->m_LoopAnimation=true;
        atc->m_AnimationDelay=0.200f;
    }else if(vel.x!=0.0f){ //mario is stopping
        desired_vel.x=vel.x*0.9f;
        if(std::abs(desired_vel.x)<0.1f){
            desired_vel.x=0.0f;
        }
        atc->m_AnimationRow=MARIO_STOPPING;
        atc->m_AnimationIndex=0;
        if(vel.x<0){
            atc->m_FlipX=false;
        }else{
            atc->m_FlipX=true;
        }
        atc->m_PlayAnimation=false;
    }

    if(std::abs(desired_vel.x)==0.0f && std::abs(desired_vel.y)==0.0f){ //mario idle
        atc->m_AnimationRow=MARIO_IDLE;
        atc->m_AnimationIndex=0;
        atc->m_PlayAnimation=false;
        atc->m_FlipX=flip;
    }

    static double jump_time=0.0;
    bool jumped=false;

    if(w_key.current==BUTTON_DOWN){ //jump
        if(!marioIsInAir){
            jump_time=glfwGetTime();
        }
        if(glfwGetTime()-jump_time<0.4f){
            desired_vel.y=std::max(vel.y+0.1f,3.0f);
            jumped=true;
        }
        marioIsInAir=true;
    }

    if(marioIsInAir){
        atc->m_AnimationRow=MARIO_JUMP;
        atc->m_AnimationIndex=0;
        atc->m_PlayAnimation=false;

        float epsilon=0.000001f;
        if(std::abs(vel.x)<epsilon){
            vel.x=0;
        }
        if(vel.x<0){
            atc->m_FlipX=true;
        }else if(vel.x>0){
            atc->m_FlipX=false;
        }else{
            atc->m_FlipX=flip;
        }
    }

    Vec2 vel_change=Vec2(desired_vel.x-vel.x,desired_vel.y-vel.y);
    b2Vec2 impulse=rb->m_RuntimeBody->GetMass()*b2Vec2(vel_change.x,vel_change.y);

    if(jumped){
        rb->m_RuntimeBody->ApplyLinearImpulseToCenter(impulse,true);
    }else{
        rb->m_RuntimeBody->ApplyLinearImpulseToCenter(b2Vec2(impulse.x,0.0f),true);
    }

    //scene->GetCamera().SetPosition({std::clamp(e->m_X-5.0f+atc->m_Width/2.0f,0.0f,MAP_WIDTH-Window::MAX_WIDTH),scene->GetCamera().GetPosition().y});
    
    //for debugging. to remove later
    static float camera_x=48.0f;
    
    scene->GetCamera().SetPosition({camera_x,scene->GetCamera().GetPosition().y});

    if(INPUT->GetKey(KEY_LEFT).current==BUTTON_DOWN){
        camera_x-=0.1f;
    }
    if(INPUT->GetKey(KEY_RIGHT).current==BUTTON_DOWN){
        camera_x+=0.1f;
    }
}

void GoombaUpdate(Scene *scene,NativeScriptComponent *nsc,float frame_time){
    RigidbodyComponent *rb=scene->GetComponent<RigidbodyComponent>(nsc->m_UID);
    
    Vec2 desired_vel;

    if(goombas_direction[nsc->m_UID]){
        desired_vel.x=0.5f;
    }else{
        desired_vel.x=-0.5f;
    }

    b2Vec2 vel=rb->m_RuntimeBody->GetLinearVelocity();
    Vec2 vel_change=Vec2(desired_vel.x-vel.x,0.0f);
    b2Vec2 impulse=rb->m_RuntimeBody->GetMass()*b2Vec2(vel_change.x,0.0f);

    rb->m_RuntimeBody->ApplyLinearImpulseToCenter(impulse,true);
}