#include <pch.hpp> 
#include <scene_buttons.hpp>
#include <utilities.hpp>
#include <editor.hpp>

/**
 * generates the application source code
*/
void GenerateApplication(const std::string &path,std::vector<std::string>&scene_names,std::vector<std::vector<std::pair<std::string,uint32_t>>>&script_components,
    std::string &window_name,unsigned int width,unsigned int height,unsigned int fullscreen_width,
    unsigned int fullscreen_height,std::string &scene_path,TonemapType tonemap,float gamma,float exposure,bool resizable=false){
    FILE *fdest=fopen((path+"/temp/data.cpp").c_str(),"w");
    if(fdest==NULL){
        perror(("Failed to open file "+path+"/temp/data.cpp").c_str());
        return;
    }
    
    fprintf(fdest,"#include <pch.hpp>\n");
    fprintf(fdest,"#include <scene.hpp>\n\n");
    fprintf(fdest,"const char *WINDOW_NAME=\"%s\";\n",window_name.c_str());
    fprintf(fdest,"unsigned int WINDOW_WIDTH=%u;\n",width);
    fprintf(fdest,"unsigned int WINDOW_HEIGHT=%u;\n",height);
    fprintf(fdest,"unsigned int FULLSCREEN_WIDTH=%u;\n",fullscreen_width);
    fprintf(fdest,"unsigned int FULLSCREEN_HEIGHT=%u;\n",fullscreen_height);
    fprintf(fdest,"std::string SCENE_PATH=\"resources/scenes/%s\";\n",scene_path.c_str());
    fprintf(fdest,"bool RESIZABLE=%s;\n",((resizable)?"true":"false"));
    fprintf(fdest,"TonemapType TONE_MAP_TYPE=TonemapType::%s;\n",TonemapTypeToString(tonemap).c_str());
    fprintf(fdest,"float GAMMA=%f;\n",gamma);
    fprintf(fdest,"float EXPOSURE=%f;\n",exposure);

    std::unordered_map<std::string,bool> fn_names;

    for(int i=0;i<scene_names.size();i++){
        for(auto &[fn_name,id]:script_components[i]){ //generate the function prototypes for the native scripts
            if(fn_names.find(fn_name)==fn_names.end()){
                fn_names[fn_name]=true;
            }else{
                continue;
            }

            if(!fn_name.empty()){
                fprintf(fdest,"extern void %s(Scene *scene,NativeScriptComponent *nsc,float frame_time);\n",fn_name.c_str());
            }
        }
    }

    fprintf(fdest,"\nvoid LoadScripts(Scene *scene){\n");

    int j=0;
    for(int i=0;i<scene_names.size();i++){
        fprintf(fdest,"\tif(scene->GetName()==\"%s\"){\n",scene_names[i].c_str());
        for(auto &[fn_name,id]:script_components[i]){ //bind the function pointers to the native scripts
            if(!fn_name.empty()){
                fprintf(fdest,"\t\tscene->AddComponent<NativeScriptComponent>((uint32_t)%u);\n",id);
                fprintf(fdest,"\t\tauto nsc%d=scene->GetComponent<NativeScriptComponent>((uint32_t)%u)->OnUpdate=%s;\n",j,id,fn_name.c_str());
                j++;
            }
        }
        fprintf(fdest,"\t}\n");
    }

    fprintf(fdest,"}");

    fclose(fdest);
}

void SceneButtons::PlayButton(const std::string &path,const std::string &editor_path,std::vector<std::string>&scene_names,std::vector<std::vector<std::pair<std::string,uint32_t>>>&script_components){
    static std::thread applicationThread;
    static std::atomic<bool> isApplicationRunning=false;

    if(isApplicationRunning.load()){ //application is already running, return
        return;
    }

    
    isApplicationRunning.store(true);
    applicationThread=std::thread([&path,&editor_path,&isApplicationRunning,&script_components,&scene_names](){ //run the application in another thread
        GenerateApplication(editor_path,scene_names,script_components,WINDOW_NAME,WINDOW_WIDTH,WINDOW_HEIGHT,FULLSCREEN_WIDTH,FULLSCREEN_HEIGHT,
            SCENE_PATH,RENDERER->GetTonemapType(),RENDERER->GetGamma(),RENDERER->GetExposure(),RESIZABLE);
        ExecuteCommand("rm -r resources/scripts/"); //remove all the previous scripts
        ExecuteCommand("cp -r "+path+"/resources/scripts resources/scripts/"); //get all the scripts from the project
        printf("%s\n",ExecuteCommand("cd application && premake5 gmake2 --file=application_premake.lua").c_str());
        ExecuteCommand("mkdir -p temp");
        #if defined(RELEASE)
            printf("%s\n",ExecuteCommand("cd lib && make config=release").c_str()); //compile the engine
            printf("%s\n",ExecuteCommand("cd temp && make config=release").c_str()); //compile the application
            RENDERER->CreateShaders(); //generate the shaders
            ExecuteCommand("rm -r "+path+"/resources/shaders/");
            ExecuteCommand("cp -r resources/shaders/ "+path+"/resources/shaders/"); //copy the shaders to the project folder
            ExecuteCommand("rm -r "+path+"/bin/Release/");
            ExecuteCommand("mkdir -p "+path+"/bin/Release/");
            ExecuteCommand("cp -r bin/Release/Application "+path+"/bin/Release/Application"); //copy the binaries to the project folder
            system(("cd "+path+" && bin/Release/Application").c_str()); //run the application
        #elif defined(DEBUG)
            printf("%s\n",ExecuteCommand("cd lib && make config=debug").c_str()); //compile the engine
            printf("%s\n",ExecuteCommand("cd temp && make config=debug").c_str()); //compile the application
            RENDERER->CreateShaders(); //generate the shaders
            ExecuteCommand("rm -r "+path+"/resources/shaders/");
            ExecuteCommand("cp -r resources/shaders/ "+path+"/resources/shaders/"); //copy the shaders to the project folder
            ExecuteCommand("rm -r "+path+"/bin/Debug/");
            ExecuteCommand("mkdir -p "+path+"/bin/Debug/");
            ExecuteCommand("cp -r bin/Debug/Application "+path+"/bin/Debug/Application"); //copy the binaries to the project folder
            system(("cd "+path+" && bin/Debug/Application").c_str()); //run the application
        #elif defined(ENABLE_PROFILING)
            printf("%s\n",ExecuteCommand("cd lib && make config=profile").c_str()); //compile the engine
            printf("%s\n",ExecuteCommand("cd temp && make config=profile").c_str()); //compile the application
            RENDERER->CreateShaders(); //generate the shaders
            ExecuteCommand("rm -r "+path+"/resources/shaders/");
            ExecuteCommand("cp -r resources/shaders/ "+path+"/resources/shaders/"); //copy the shaders to the project folder
            ExecuteCommand("rm -r "+path+"/bin/Profile/");
            ExecuteCommand("mkdir -p "+path+"/bin/Profile/");
            ExecuteCommand("cp -r bin/Profile/Application "+path+"/bin/Profile/Application"); //copy the binaries to the project folder
            system(("cd "+path+" && bin/Profile/Application").c_str()); //run the application
        #endif

        isApplicationRunning.store(false);
    });
    applicationThread.detach(); //detach the thread
}