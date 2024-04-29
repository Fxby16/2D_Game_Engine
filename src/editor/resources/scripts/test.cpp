#include <pch.hpp>
#include <scene.hpp>
#include <buttons.hpp>
#include <window.hpp>
#include <application.hpp>

class ContactListener : public b2ContactListener{
    void BeginContact(b2Contact *contact) override{
        uint32_t uid1=(uint32_t)contact->GetFixtureA()->GetUserData().pointer;
        uint32_t uid2=(uint32_t)contact->GetFixtureB()->GetUserData().pointer;

        printf("Entity %d started colliding with entity %d\n",uid1,uid2);
    }

    void EndContact(b2Contact *contact) override{
        uint32_t uid1=(uint32_t)contact->GetFixtureA()->GetUserData().pointer;
        uint32_t uid2=(uint32_t)contact->GetFixtureB()->GetUserData().pointer;

        printf("Entity %d stopped colliding with entity %d\n",uid1,uid2);
    }
};

ContactListener contact_listener;

using namespace Window;

void Entity1Update(Scene *scene,NativeScriptComponent *nsc,float frame_time){
    Vec2 speed;
    
    if(GetKeyState(KEY_W,BUTTON_DOWN)){
        speed.y+=5.0f;
    }
    if(GetKeyState(KEY_S,BUTTON_DOWN)){
        speed.y-=5.0f;
    }
    if(GetKeyState(KEY_A,BUTTON_DOWN)){
        speed.x-=10.0f;
    }
    if(GetKeyState(KEY_D,BUTTON_DOWN)){
        speed.x+=10.0f;
    }
    if(speed.x!=0.0f || speed.y!=0.0f)
        scene->MoveEntity(nsc->m_UID,speed.x,speed.y);
}

void BeforeUpdate(double frame_time,Application *app){
    KeyState k=INPUT->GetKey(KEY_K);

    if(k.current && !k.previous){
        app->LoadScene("test2.scene");   
    }
}

void AfterUpdate(double frame_time,Application *app){
    
}

void OnCreate(Application *app){
    app->GetScene()->GetPhysicsWorld()->SetContactListener(&contact_listener);
}

void OnDestroy(Application *app){

}