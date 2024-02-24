#include <pch.hpp> 
#include <scene_buttons.hpp>
#include <utilities.hpp>

void GenerateApplication(std::vector<std::pair<std::string,uint32_t>>&script_components,const char *window_name,unsigned int width,unsigned int height,float fullscreen_width,float fullscreen_height,std::string &scene_path,bool resizable=false){
    FILE *fdest=fopen("temp/data.cpp","w");
    if(fdest==NULL){
        perror("Failed to open file temp/data.cpp");
        return;
    }
    
    fprintf(fdest,"#include <pch.hpp>\n");
    fprintf(fdest,"#include <scene.hpp>\n\n");
    fprintf(fdest,"const char *WINDOW_NAME=\"%s\";\n",window_name);
    fprintf(fdest,"unsigned int WINDOW_WIDTH=%u;\n",width);
    fprintf(fdest,"unsigned int WINDOW_HEIGHT=%u;\n",height);
    fprintf(fdest,"float FULLSCREEN_WIDTH=%f;\n",fullscreen_width);
    fprintf(fdest,"float FULLSCREEN_HEIGHT=%f;\n",fullscreen_height);
    fprintf(fdest,"std::string SCENE_PATH=\"%s\";\n",scene_path.c_str());
    fprintf(fdest,"bool RESIZABLE=%s;\n",((resizable)?"true":"false"));

    for(auto &[fn_name,id]:script_components){
        if(!fn_name.empty()){
            fprintf(fdest,"extern void %s(Scene *scene,NativeScriptComponent *nsc,float frame_time);\n",fn_name.c_str());
        }
    }

    fprintf(fdest,"\nvoid LoadScripts(Scene *scene){\n");
    
    int i=0;
    for(auto &[fn_name,id]:script_components){
        if(!fn_name.empty()){
            fprintf(fdest,"\tscene->AddComponent<NativeScriptComponent>((uint32_t)%u);\n",id);
            fprintf(fdest,"\tauto nsc%d=scene->GetComponent<NativeScriptComponent>((uint32_t)%u)->OnUpdate=%s;\n",i,id,fn_name.c_str());
            i++;
        }
    }

    fprintf(fdest,"}");

    fclose(fdest);
}

void SceneButtons::PlayButton(std::vector<std::pair<std::string,uint32_t>>&script_components){
    static std::thread applicationThread;
    static std::atomic<bool> isApplicationRunning=false;

    if(isApplicationRunning.load()){
        return;
    }
    
    isApplicationRunning.store(true);
    applicationThread=std::thread([&isApplicationRunning,&script_components](){
        std::string scene_path="test.scene";
        ExecuteCommand("mkdir -p temp");
        GenerateApplication(script_components,"Test",800,600,1920,1080,scene_path,true);
        printf("%s\n",ExecuteCommand("cd application && premake5 gmake2 --file=application_premake.lua").c_str());
        printf("%s\n",ExecuteCommand("cd lib && make config=release").c_str());
        printf("%s\n",ExecuteCommand("cd temp && make config=release").c_str());

        system("bin/Release/Application");
        isApplicationRunning.store(false);
        printf("%s\n",ExecuteCommand("rm bin/Release/Application").c_str());
    });
    applicationThread.detach();
}