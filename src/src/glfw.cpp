#include <glfw.hpp>
#include <cstdio>

bool isFullScreen=false;

void GLAPIENTRY MessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const void* userParam){
    if(type==GL_DEBUG_TYPE_ERROR)
        fprintf(stderr,"[[GL ERROR]]  Type: 0x%x  Severity: 0x%x  Message: %s\n",type,severity,message);
}

void GLAPIENTRY FramebufferSizeCallback(GLFWwindow *window,int width,int height){
    glViewport(0,0,width,height);
    framebuffer_update=true;
    proj_update=true;
    SCREEN_WIDTH=width;
    SCREEN_HEIGHT=height;
}  

void GLAPIENTRY KeyCallback(GLFWwindow *window,int key,int scancode,int action,int mode){
    if(key==GLFW_KEY_Q && action==GLFW_PRESS)
        glfwSetWindowShouldClose(window,true);

    if(key==GLFW_KEY_F && action==GLFW_PRESS)
        ToggleFullScreen();
}

int InitGlfwWindow(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    #endif
    glfwWindowHint(GLFW_RESIZABLE,false);

    window=glfwCreateWindow(SCREEN_WIDTH,SCREEN_HEIGHT,"Test",nullptr,nullptr);
    if(window==NULL){
        fprintf(stderr,"Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); //v-sync

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        fprintf(stderr,"Failed to initialize GLAD\n");
        return -1;
    } 

    glfwSetFramebufferSizeCallback(window,FramebufferSizeCallback);
    glfwSetKeyCallback(window,KeyCallback);
    //enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback,0);

    printf("%s\n%s\n",(char *)glGetString(GL_VERSION),(char *)glGetString(GL_RENDERER));

    glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    return 0;
}

void ToggleFullScreen(){
    const GLFWvidmode *mode=glfwGetVideoMode(glfwGetPrimaryMonitor());
    if(isFullScreen)
        glfwSetWindowMonitor(window,nullptr,mode->width/2-BASE_SCREEN_WIDTH/2,mode->height/2-BASE_SCREEN_HEIGHT/2,BASE_SCREEN_WIDTH,BASE_SCREEN_HEIGHT,GLFW_DONT_CARE);
    else
        glfwSetWindowMonitor(window,glfwGetPrimaryMonitor(),0,0,mode->width,mode->height,GLFW_DONT_CARE);
    isFullScreen=!isFullScreen;
    WindowInfo::isFullScreen=isFullScreen;
}