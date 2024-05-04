#include <pch.hpp>
#include <scene.hpp>
#include <buttons.hpp>
#include <window.hpp>
#include <application.hpp>
#include <sceneserializer.hpp>

using namespace Window;

extern SceneSerializer sceneserializer;

void BeforeUpdate(double frame_time,Application *app){

}

void AfterUpdate(double frame_time,Application *app){
    
}

void OnCreate(Application *app){
    Scene *scene=app->GetScene();

    for(int i=0;i<2;i++){
        for(int j=0;j<208;j++){
            uint32_t uid=scene->AddEntity();
            scene->SetEntityPosition(uid,j*0.625f,i*0.625f);
            scene->AddComponent<AnimatedTextureComponent>(uid,"resources/textures/blocks.png",16,16,GL_NEAREST,GL_NEAREST,0.625f,0.625f,0,false,false,0,0,4,true,false,false);
        }
    }

    std::vector<std::pair<std::string,uint32_t>> script_components;
    sceneserializer.Serialize("resources/scenes/"+app->GetSceneName(),script_components);
}

void OnDestroy(Application *app){

}