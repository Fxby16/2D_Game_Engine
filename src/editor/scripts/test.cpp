#include <scene.hpp>
#include <buttons.hpp>
#include <window.hpp>

using namespace Window;

void Entity1Update(Scene *scene,NativeScriptComponent *nsc,float frame_time){
    Vec2 speed;
    
    if(GetKeyState(KEY_W,BUTTON_DOWN)){
        speed.y+=10.0f;
    }
    if(GetKeyState(KEY_S,BUTTON_DOWN)){
        speed.y-=10.0f;
    }
    if(GetKeyState(KEY_A,BUTTON_DOWN)){
        speed.x-=10.0f;
    }
    if(GetKeyState(KEY_D,BUTTON_DOWN)){
        speed.x+=10.0f;
    }
    scene->MoveEntity(nsc->m_UID,speed.x,speed.y);
}
