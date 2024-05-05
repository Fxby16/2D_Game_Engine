#include <pch.hpp>
#include <scene.hpp>
#include <buttons.hpp>
#include <window.hpp>
#include <application.hpp>
#include <sceneserializer.hpp>

#define QUESTION_BLOCK_GROUP 1
#define MAP_GROUP 2
#define MARIO_GROUP 3

#define MARIO_IDLE 3
#define MARIO_RUN 2
#define MARIO_JUMP 1
#define MARIO_STOPPING 0

using namespace Window;

bool marioIsInAir=false;

Scene *scene=nullptr;

class ContactListener : public b2ContactListener{
    void BeginContact(b2Contact *contact) override{
        b2Fixture *fixtureA=contact->GetFixtureA();
        b2Fixture *fixtureB=contact->GetFixtureB();

        uint32_t uid_a=(uint32_t)fixtureA->GetUserData().pointer;
        uint32_t uid_b=(uint32_t)fixtureB->GetUserData().pointer;

        Entity *entity_a=scene->GetEntity(uid_a);
        Entity *entity_b=scene->GetEntity(uid_b);

        BoxColliderComponent *bcc_a=scene->GetComponent<BoxColliderComponent>(uid_a);
        BoxColliderComponent *bcc_b=scene->GetComponent<BoxColliderComponent>(uid_b);

        //printf("Contact between %u and %u\n",uid_a,uid_b);

        if(entity_a->m_Group==MARIO_GROUP){
            if(entity_a->m_Y>=entity_b->m_Y+bcc_b->m_YOffset+bcc_b->m_Height){
                marioIsInAir=false;
            }
        }else if(entity_b->m_Group==MARIO_GROUP){
            if(entity_b->m_Y>=entity_a->m_Y+bcc_a->m_YOffset+bcc_a->m_Height){
                marioIsInAir=false;
            }
        }
    }

    void EndContact(b2Contact *contact) override{
    }
};

ContactListener contactlistener;

extern SceneSerializer sceneserializer;

void BeforeUpdate(double frame_time,Application *app){
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
    RigidbodyComponent *rb=scene->GetComponent<RigidbodyComponent>(nsc->m_UID);
    AnimatedTextureComponent *atc=scene->GetComponent<AnimatedTextureComponent>(nsc->m_UID);
    Entity *e=scene->GetEntity(nsc->m_UID);

    b2Vec2 vel=rb->m_RuntimeBody->GetLinearVelocity();
    Vec2 desired_vel={0.0f,0.0f};

    KeyState a_key=INPUT->GetKey(KEY_A);
    KeyState d_key=INPUT->GetKey(KEY_D);
    KeyState w_key=INPUT->GetKey(KEY_W);

    if(a_key.current==BUTTON_DOWN && d_key.current!=BUTTON_DOWN){ //mario is moving left
        desired_vel.x=std::max(vel.x-0.1f,-5.0f);
        atc->m_FlipX=true;
        atc->m_AnimationRow=MARIO_RUN;
        atc->m_PlayAnimation=true;
        atc->m_LoopAnimation=true;
        atc->m_AnimationDelay=0.200f;
    }else if(d_key.current==BUTTON_DOWN && a_key.current!=BUTTON_DOWN){ //mario is moving right
        desired_vel.x=std::min(vel.x+0.1f,5.0f);
        atc->m_FlipX=false;
        atc->m_AnimationRow=MARIO_RUN;
        atc->m_PlayAnimation=true;
        atc->m_LoopAnimation=true;
        atc->m_AnimationDelay=0.200f;
    }else{ //mario is stopping
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
        atc->m_FlipX=false;
    }

    printf("Vel.x: %f\n",vel.x);
    if(w_key.current==BUTTON_DOWN){ //jump
        desired_vel.y=std::max(vel.y+0.1f,5.0f);
        float epsilon=0.000001f;
        if(std::abs(vel.x)<epsilon){
            vel.x=0;
        }
        if(vel.x<0){
            atc->m_FlipX=true;
        }else{
            atc->m_FlipX=false;
        }
        marioIsInAir=true;
    }

    if(marioIsInAir){
        atc->m_AnimationRow=MARIO_JUMP;
        atc->m_AnimationIndex=0;
        atc->m_PlayAnimation=false;
    }

    Vec2 vel_change=Vec2(desired_vel.x-vel.x,desired_vel.y-vel.y);
    b2Vec2 impulse=rb->m_RuntimeBody->GetMass()*b2Vec2(vel_change.x,vel_change.y);

    if(w_key.current==BUTTON_DOWN){
        rb->m_RuntimeBody->ApplyLinearImpulseToCenter(impulse,true);
    }else{
        rb->m_RuntimeBody->ApplyLinearImpulseToCenter(b2Vec2(impulse.x,0.0f),true);
    }
}