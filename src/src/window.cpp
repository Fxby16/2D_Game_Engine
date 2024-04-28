#include <pch.hpp>
#include <window.hpp>
#include <buttons.hpp>
#include <texturesmanager.hpp>
#include <fontmanager.hpp>
#include <textqueue.hpp>
#ifdef EDITOR
    #include <editor.hpp>
#endif

bool ISFULLSCREEN=false;
Renderer *RENDERER;
TextRenderer *TEXT_RENDERER;
Input *INPUT;
TexturesManager *TEXTURES_MANAGER;
FontManager *FONT_MANAGER;
TextQueue *TEXT_QUEUE;

namespace Window{
    void GLAPIENTRY MessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const void* userParam){
        if(type==GL_DEBUG_TYPE_ERROR)
            fprintf(stderr,"[[GL ERROR]]  Type: 0x%x  Severity: 0x%x  Message: %s\n",type,severity,message);
    }

    void GLAPIENTRY FramebufferSizeCallback(GLFWwindow *WINDOW,int width,int height){
        glViewport(0,0,width,height);
        FramebufferUpdate=true;
        ProjUpdate=true;
        #ifdef EDITOR
            SceneFramebufferUpdate=true;
        #endif
        float current_point_size=RENDERER->GetPointSize();
        float current_line_width=RENDERER->GetLineWidth();
        Width=width;
        Height=height;
        RENDERER->SetPointSize(current_point_size);
        RENDERER->SetLineWidth(current_line_width);
        MAX_HEIGHT=MAX_WIDTH/(Width/Height);

        RendererData *lights=RENDERER->GetLightsData();
        lights->S.Bind();
        lights->S.SetUniform1f("window_width",(float)width);

        #ifdef DEBUG
            printf("Framebuffer resized to %dx%d\n",width,height);
        #endif
    }  

    void GLAPIENTRY ErrorCallback(int error,const char *description){
        fprintf(stderr, "Error: %s\n", description);
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
        TEXT_RENDERER->DrawText("FPS: "+std::to_string(FPS)+"\nFrame Time: "+std::to_string(DeltaTime*1000.0f)+" ms",
                           0.0f,Window::MAX_HEIGHT-TEXT_RENDERER->GetTextSize("FPS: "+std::to_string(FPS)+"\nFrame Time: "+std::to_string(DeltaTime*1000.0f)+" ms",
                           0.5f).second,0.5f,1'000'000,Vec3(1.0f,1.0f,1.0f));
    }

    int InitGlfwWindow(const char *window_name,bool resizable){
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
        #endif
        glfwWindowHint(GLFW_RESIZABLE,resizable);

        Window=glfwCreateWindow(Width,Height,window_name,nullptr,nullptr);
        if(Window==NULL){
            perror("Failed to create GLFW WINDOW\n");
            glfwTerminate();
            return 1;
        }

        glfwMakeContextCurrent(Window);
        glfwSwapInterval(1); //v-sync on

        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            perror("Failed to initialize GLAD\n");
            return 1;
        } 

        glfwSetFramebufferSizeCallback(Window,FramebufferSizeCallback);
        glfwSetErrorCallback(ErrorCallback);

        #ifdef DEBUG
            glEnable(GL_DEBUG_OUTPUT);
            glDebugMessageCallback(MessageCallback,0);
        #endif

        printf("%s\n%s\n",(char *)glGetString(GL_VERSION),(char *)glGetString(GL_RENDERER));

        glViewport(0,0,Width,Height);

        RENDERER=new Renderer;
        TEXT_RENDERER=new TextRenderer("resources/fonts/open-sans/OpenSans-Regular.ttf",10,true);
        INPUT=new Input;
        TEXTURES_MANAGER=new TexturesManager;
        FONT_MANAGER=new FontManager;
        TEXT_QUEUE=new TextQueue;

        return 0;
    }

    void DeinitGlfwWindow(){
        delete RENDERER;
        delete TEXT_RENDERER;
        delete INPUT;
        delete TEXTURES_MANAGER;
        delete FONT_MANAGER;
        delete TEXT_QUEUE;
        glfwTerminate();
    }

    void ToggleFullScreen(){
        int count;
        const GLFWvidmode* modes=glfwGetVideoModes(glfwGetPrimaryMonitor(), &count);

        printf("Available modes:\n");
        for(int i=0;i<count;i++){
            printf("%d: %dx%d @ %dHz\n",i,modes[i].width,modes[i].height,modes[i].refreshRate);
        }
        if(IsFullscreen)
            glfwSetWindowMonitor(Window,nullptr,modes[count-1].width/2-BaseWidth/2,modes[count-1].height/2-BaseHeight/2,BaseWidth,BaseHeight,GLFW_DONT_CARE);
        else
            glfwSetWindowMonitor(Window,glfwGetPrimaryMonitor(),0,0,FullscreenWidth,FullscreenHeight,GLFW_DONT_CARE);
        ISFULLSCREEN=!ISFULLSCREEN;
        IsFullscreen=ISFULLSCREEN;
    }

    void ToggleVSync(){
        IsVSync=!IsVSync;
        glfwSwapInterval(IsVSync);
    }

    void SetResizable(bool resizable){
        glfwWindowHint(GLFW_RESIZABLE,resizable);
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
    float FullscreenWidth,FullscreenHeight;

    float FPS;
    float CurrentFrameTime;
    float LastFrameTime;
    float DeltaTime;
    float Alpha; //for interpolation

    bool IsFullscreen;
    bool IsVSync;
    bool ShowMetrics_;

    bool FramebufferUpdate=false;
    bool ProjUpdate=false;
    bool TextProjUpdate=false;
    #ifdef EDITOR
        bool SceneFramebufferUpdate=false;

        int DrawCalls=0;
        int VertexCount=0;
    #endif

    const float MAX_WIDTH=10.0f;
    float MAX_HEIGHT;
}