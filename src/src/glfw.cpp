#include <glfw.hpp>
#include <cstdio>

void GLAPIENTRY MessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const void* userParam){
    if(type==GL_DEBUG_TYPE_ERROR)
        fprintf(stderr,"[[GL ERROR]]  Type: 0x%x  Severity: 0x%x  Message: %s\n",type,severity,message);
}

void framebuffer_size_callback(GLFWwindow *window,int width,int height){
    glViewport(0,0,width,height);
}  

void key_callback(GLFWwindow *window,int key,int scancode,int action,int mode){
    if(key==GLFW_KEY_Q && action==GLFW_PRESS)
        glfwSetWindowShouldClose(window,true);
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

    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glfwSetKeyCallback(window,key_callback);
    //enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback,0);

    glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    //int MaxTextureImageUnits;
    //glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureImageUnits);
    return 0;
}