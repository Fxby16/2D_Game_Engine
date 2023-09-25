#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vertexarray.hpp>
#include <renderer.hpp>
#include <indexbuffer.hpp>
#include <vertexbuffer.hpp>
#include <vertexbufferlayout.hpp>
#include <texture.hpp>
#include <global.hpp>

#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void GLAPIENTRY MessageCallback(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const void* userParam){
    fprintf(stderr,"GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
    (type==GL_DEBUG_TYPE_ERROR?"** GL ERROR **":""),type,severity,message);
}

void framebuffer_size_callback(GLFWwindow *window,int width,int height){
    glViewport(0,0,width,height);
}  

void key_callback(GLFWwindow *window,int key,int scancode,int action,int mode){
    if(key==GLFW_KEY_Q && action==GLFW_PRESS)
        glfwSetWindowShouldClose(window,true);
}

int main(){{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    #endif
    glfwWindowHint(GLFW_RESIZABLE,false);

    GLFWwindow *window=glfwCreateWindow(SCREEN_WIDTH,SCREEN_HEIGHT,"Test",nullptr,nullptr);
    if(window==NULL){
        std::cout<<"Failed to create GLFW window"<< std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); //v-sync

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout<<"Failed to initialize GLAD"<< std::endl;
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

Renderer renderer;
    Renderer::ImGui_Init(window);

    renderer.AddLayout(GL_FLOAT,2,false);
    renderer.AddLayout(GL_FLOAT,2,false);
    renderer.AddLayout(GL_FLOAT,1,false);

Texture texture("resources/textures/cicciogamer89.jpg");
    texture.Bind(0);

int MaxTextureImageUnits;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureImageUnits); 
    //std::cout<<MaxTextureImageUnits<<'\n';

    while(!glfwWindowShouldClose(window)){
        renderer.Clear();

        glfwPollEvents();
        float x=0.0f,y=0.0f; //creating data for the vertex buffer
        for(int i=0;i<NUM_QUADS;i++){
            renderer.Render(x,y,10.0f,10.0f,0.0f);
            x+=10;
            if(x>=SCREEN_WIDTH){
                x=0;
                y+=10;
            }
            if(y>=SCREEN_HEIGHT){
                y=0;
                x=0;
            }
        }

        renderer.Draw();
        Renderer::ImGui_Start_Frame();
        Renderer::ImGui_Body();
        Renderer::ImGui_End_Frame();

        glfwSwapBuffers(window);
    }
}
    Renderer::ImGui_Close();

    glfwTerminate();
    return 0;
}