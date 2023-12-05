#include <glfw.hpp>
#include <stdio.h>

int InitGlfwWindow(const char *window_name){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    #endif
    glfwWindowHint(GLFW_RESIZABLE,false);

    WINDOW=glfwCreateWindow(SCREEN_WIDTH,SCREEN_HEIGHT,window_name,nullptr,nullptr);
    if(WINDOW==NULL){
        perror("Failed to create GLFW WINDOW\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(WINDOW);
    glfwSwapInterval(1); //v-sync

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        perror("Failed to initialize GLAD\n");
        return -1;
    } 

    glfwSetFramebufferSizeCallback(WINDOW,FramebufferSizeCallback);

    printf("%s\n%s\n",(char *)glGetString(GL_VERSION),(char *)glGetString(GL_RENDERER));

    glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);

    return 0;
}

void GLAPIENTRY MessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const void* userParam){
    if(type==GL_DEBUG_TYPE_ERROR)
        fprintf(stderr,"[[GL ERROR]]  Type: 0x%x  Severity: 0x%x  Message: %s\n",type,severity,message);
}

void GLAPIENTRY FramebufferSizeCallback(GLFWwindow *WINDOW,int width,int height){
    glViewport(0,0,width,height);
}  