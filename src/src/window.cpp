#include <window.hpp>
#include <cstdio>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <buttons.hpp>

bool ISFULLSCREEN=false;
Renderer *RENDERER;
std::vector<TextRenderer*> TEXT_RENDERERS;

namespace Window{
    void GLAPIENTRY MessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const void* userParam){
        if(type==GL_DEBUG_TYPE_ERROR)
            fprintf(stderr,"[[GL ERROR]]  Type: 0x%x  Severity: 0x%x  Message: %s\n",type,severity,message);
    }

    void GLAPIENTRY FramebufferSizeCallback(GLFWwindow *WINDOW,int width,int height){
        glViewport(0,0,width,height);
        FramebufferUpdate=true;
        ProjUpdate=true;
        float current_point_size=RENDERER->GetPointSize();
        float current_line_width=RENDERER->GetLineWidth();
        Width=width;
        Height=height;
        RENDERER->SetPointSize(current_point_size);
        RENDERER->SetLineWidth(current_line_width);
        MAX_HEIGHT=MAX_WIDTH/(Width/Height);
    }  

    void GLAPIENTRY KeyCallback(GLFWwindow *window,int key,int scancode,int action,int mods){
        if(key==KEY_F11 && action==BUTTON_DOWN)
            ToggleFullScreen();
        if(key==KEY_F10 && action==BUTTON_DOWN)
            ToggleVSync();
        if(key==KEY_F9 && action==BUTTON_DOWN)
            ShowMetrics_=!ShowMetrics_;
        if(key==KEY_Q && action==BUTTON_DOWN)
            glfwSetWindowShouldClose(window,true);
    }

    void GetMousePos(double *x,double *y){
        glfwGetCursorPos(Window,x,y);
        *y=Height-(*y);
    }

    void ShowMetrics(){
        static int FPS=0;
        static double last_time=0.0;
        double current_time=glfwGetTime();
        if(current_time-last_time>=0.5){
            last_time=current_time;
            FPS=1.0/DeltaTime;
        }
        TEXT_RENDERERS[0]->DrawText("FPS: "+std::to_string(FPS)+"\nFrame Time: "+std::to_string(DeltaTime*1000.0f)+" ms",
                           0.0f,Window::MAX_HEIGHT-TEXT_RENDERERS[0]->GetTextSize("FPS: "+std::to_string(FPS)+"\nFrame Time: "+std::to_string(DeltaTime*1000.0f)+" ms",
                           0.5f).second,0.5f,Vec3(1.0f,1.0f,1.0f));
    }

    int InitGlfwWindow(const char *window_name){
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
        #endif
        glfwWindowHint(GLFW_RESIZABLE,false);

        Window=glfwCreateWindow(Width,Height,window_name,nullptr,nullptr);
        if(Window==NULL){
            perror("Failed to create GLFW WINDOW\n");
            glfwTerminate();
            return 1;
        }

        glfwMakeContextCurrent(Window);
        glfwSwapInterval(1); //v-sync

        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            perror("Failed to initialize GLAD\n");
            return 1;
        } 

        glfwSetFramebufferSizeCallback(Window,FramebufferSizeCallback);
        glfwSetKeyCallback(Window,KeyCallback);

        #ifdef DEBUG
            glEnable(GL_DEBUG_OUTPUT);
            glDebugMessageCallback(MessageCallback,0);
        #endif

        printf("%s\n%s\n",(char *)glGetString(GL_VERSION),(char *)glGetString(GL_RENDERER));

        glViewport(0,0,Width,Height);

        RENDERER=new Renderer;
        TEXT_RENDERERS.push_back(new TextRenderer("resources/fonts/open-sans/OpenSans-Regular.ttf",true));

        return 0;
    }

    void DeinitGlfwWindow(){
        delete RENDERER;
        for(int i=0;i<TEXT_RENDERERS.size();i++)
            delete TEXT_RENDERERS[i];
        glfwTerminate();
    }

    void ToggleFullScreen(){
        const GLFWvidmode *mode=glfwGetVideoMode(glfwGetPrimaryMonitor());
        if(IsFullscreen)
            glfwSetWindowMonitor(Window,nullptr,mode->width/2-BaseWidth/2,mode->height/2-BaseHeight/2,BaseWidth,BaseHeight,GLFW_DONT_CARE);
        else
            glfwSetWindowMonitor(Window,glfwGetPrimaryMonitor(),0,0,mode->width,mode->height,GLFW_DONT_CARE);
        ISFULLSCREEN=!ISFULLSCREEN;
        IsFullscreen=ISFULLSCREEN;
    }

    void ToggleVSync(){
        IsVSync=!IsVSync;
        glfwSwapInterval(IsVSync);
    }

    int parseLine(char *line){
        int i=0;
        while(*line<'0' || *line>'9') 
            line++;
        while(*line>='0'&&*line<='9'){
            i=i*10+(*line-'0');
            line++;
        }
        return i;
    }

    /**
     * Get value from /proc/meminfo
     * \param value a string containing the value name, for example `MemTotal`.
     * \return the value in kb
    */
    int getMemInfoValue(const char *value){
        FILE *file=fopen("/proc/meminfo","r");
        int result=-1;
        char line[128];

        while(fgets(line,128,file)!=NULL){
            if(strncmp(line,value,strlen(value))==0){
                result=parseLine(line);
                break;
            }
        }
        fclose(file);
        return result;
    }

    /**
     * Get value from /proc/self/status
     * \param value a string containing the value name, for example `VmRSS`.
     * \return the value in kb
    */
    int getStatusValue(const char *value){
        FILE *file=fopen("/proc/self/status","r");
        int result=-1;
        char line[128];

        while(fgets(line,128,file)!=NULL){
            if(strncmp(line,value,strlen(value))==0){
                result=parseLine(line);
                break;
            }
        }
        fclose(file);
        return result;
    }

    void PrintDebugInfo(){
        float totalMem=static_cast<float>(getMemInfoValue("MemTotal"))/1024.0f;
        float availableMem=static_cast<float>(getMemInfoValue("MemAvailable"))/1024.0f;
        //float SwapTotal=static_cast<float>(getMemInfoValue("SwapTotal"))/1024.0f;
        //float SwapFree=static_cast<float>(getMemInfoValue("SwapFree"))/1024.0f;
        float residentMem=static_cast<float>(getStatusValue("VmRSS"))/1024.0f;
        printf("RAM: %.2f/%.2f MB\n",totalMem-availableMem,totalMem);
        //printf("Swap: %.2f/%.2f MB\n",SwapTotal-SwapFree,SwapTotal);
        printf("Resident Memory: %.2f MB\n",residentMem);

        putchar('\n');
    }

    bool GetKeyState(int key,int state){
        return glfwGetKey(Window,key)==state;
    }

    bool GetMouseButtonState(int button,int state){
        return glfwGetMouseButton(Window,button)==state;
    }

    GLFWwindow *Window;

    float Width,Height;
    float BaseWidth,BaseHeight;

    float FPS;
    float CurrentFrameTime;
    float LastFrameTime;
    float DeltaTime;
    float Alpha; //for interpolation

    bool IsFullscreen;
    bool IsVSync;
    bool ShowMetrics_;

    bool FramebufferUpdate;
    bool ProjUpdate;
    bool TextProjUpdate;

    const float MAX_WIDTH=10.0f;
    float MAX_HEIGHT;
}