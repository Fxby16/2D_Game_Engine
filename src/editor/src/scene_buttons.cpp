#include <pch.hpp> 
#include <scene_buttons.hpp>
#include <utilities.hpp>
#include <editor.hpp>

/**
 * generates the application source code
*/
void GenerateApplication(std::vector<std::pair<std::string,uint32_t>>&script_components,
    std::string &window_name,unsigned int width,unsigned int height,unsigned int fullscreen_width,
    unsigned int fullscreen_height,std::string &scene_path,TonemapType tonemap,float gamma,float exposure,bool resizable=false){
    FILE *fdest=fopen("temp/data.cpp","w");
    if(fdest==NULL){
        perror("Failed to open file temp/data.cpp");
        return;
    }
    
    fprintf(fdest,"#include <pch.hpp>\n");
    fprintf(fdest,"#include <scene.hpp>\n\n");
    fprintf(fdest,"const char *WINDOW_NAME=\"%s\";\n",window_name.c_str());
    fprintf(fdest,"unsigned int WINDOW_WIDTH=%u;\n",width);
    fprintf(fdest,"unsigned int WINDOW_HEIGHT=%u;\n",height);
    fprintf(fdest,"unsigned int FULLSCREEN_WIDTH=%u;\n",fullscreen_width);
    fprintf(fdest,"unsigned int FULLSCREEN_HEIGHT=%u;\n",fullscreen_height);
    fprintf(fdest,"std::string SCENE_PATH=\"%s\";\n",scene_path.c_str());
    fprintf(fdest,"bool RESIZABLE=%s;\n",((resizable)?"true":"false"));
    fprintf(fdest,"TonemapType TONE_MAP_TYPE=TonemapType::%s;\n",TonemapTypeToString(tonemap).c_str());
    fprintf(fdest,"float GAMMA=%f;\n",gamma);
    fprintf(fdest,"float EXPOSURE=%f;\n",exposure);

    for(auto &[fn_name,id]:script_components){ //generate the function prototypes for the native scripts
        if(!fn_name.empty()){
            fprintf(fdest,"extern void %s(Scene *scene,NativeScriptComponent *nsc,float frame_time);\n",fn_name.c_str());
        }
    }

    fprintf(fdest,"\nvoid LoadScripts(Scene *scene){\n");
    
    int i=0;
    for(auto &[fn_name,id]:script_components){ //bind the function pointers to the native scripts
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

    if(isApplicationRunning.load()){ //application is already running, return
        return;
    }
    
    isApplicationRunning.store(true);
    applicationThread=std::thread([&isApplicationRunning,&script_components](){ //run the application in another thread
        std::string scene_path="test.scene";
        ExecuteCommand("mkdir -p temp");
        GenerateApplication(script_components,WINDOW_NAME,WINDOW_WIDTH,WINDOW_HEIGHT,FULLSCREEN_WIDTH,FULLSCREEN_HEIGHT,
            SCENE_PATH,RENDERER->GetTonemapType(),RENDERER->GetGamma(),RENDERER->GetExposure(),RESIZABLE);
        printf("%s\n",ExecuteCommand("cd application && premake5 gmake2 --file=application_premake.lua").c_str());
        #if defined(RELEASE)
            printf("%s\n",ExecuteCommand("cd lib && make config=release").c_str());
            printf("%s\n",ExecuteCommand("cd temp && make config=release").c_str());
            ExecuteCommand("mkdir -p bin/Release/resources");
            ExecuteCommand("cp -r resources/* bin/Release/resources");
            char command[256];
            command[0]='\0';
            strcat(command,"cp ");
            strcat(command,SCENE_PATH.c_str());
            strcat(command," bin/Release/");
            strcat(command,SCENE_PATH.c_str());
            printf("%s\n",ExecuteCommand(command).c_str());
            RENDERER->CreateShaders();
            system("cd bin/Release && ./Application");
        #elif defined(DEBUG)
            printf("%s\n",ExecuteCommand("cd lib && make config=debug").c_str());
            printf("%s\n",ExecuteCommand("cd temp && make config=debug").c_str());
            ExecuteCommand("mkdir -p bin/Debug/resources");
            ExecuteCommand("cp -r resources/* bin/Debug/resources");
            char command[256];
            command[0]='\0';
            strcat(command,"cp ");
            strcat(command,SCENE_PATH.c_str());
            strcat(command," bin/Debug/");
            strcat(command,SCENE_PATH.c_str());
            printf("%s\n",ExecuteCommand(command).c_str());
            RENDERER->CreateShaders();
            system("cd bin/Debug && ./Application");
        #elif defined(ENABLE_PROFILING)
            printf("%s\n",ExecuteCommand("cd lib && make config=profile").c_str());
            printf("%s\n",ExecuteCommand("cd temp && make config=profile").c_str());
            ExecuteCommand("mkdir -p bin/Profile/resources");
            ExecuteCommand("cp -r resources/* bin/Profile/resources");
            char command[256];
            command[0]='\0';
            strcat(command,"cp ");
            strcat(command,SCENE_PATH.c_str());
            strcat(command," bin/Profile/");
            strcat(command,SCENE_PATH.c_str());
            printf("%s\n",ExecuteCommand(command).c_str());
            RENDERER->CreateShaders();
            system("cd bin/Profile && ./Application");
        #endif

        isApplicationRunning.store(false);
    });
    applicationThread.detach(); //detach the thread
}