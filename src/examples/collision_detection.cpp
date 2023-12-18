#include <game.hpp>

class Example : public Game{
private:
    Entity e0;
    Entity e1;
    Entity e2;
    Entity e3;
    Entity e4;
public:
    Example(const char *window_name,unsigned int width,unsigned int height,bool imgui=true): Game(window_name,width,height,imgui),e0("resources/textures/smurf_cat.jpg",0,0,100,100),
    e1("resources/textures/cicciogamer89.jpg",150,150,100,100),e2("resources/textures/cicciogamer89.jpg",300,300,100,100),
    e3("resources/textures/cicciogamer89.jpg",450,450,100,100),e4("resources/textures/cicciogamer89.jpg",600,600,100,100)
    {
        e0.AddCollider();
        e1.AddCollider(true);
        e2.AddCollider(true);
        e3.AddCollider(true);
        e4.AddCollider(true);
    }

    void OnUpdate(const double frame_time) override{
        m_Renderer->Clear();

        int x_offset=0,y_offset=0;
        if(glfwGetKey(WINDOW,GLFW_KEY_W)==GLFW_PRESS)
            y_offset+=20;
        if(glfwGetKey(WINDOW,GLFW_KEY_A)==GLFW_PRESS)
            x_offset-=20;
        if(glfwGetKey(WINDOW,GLFW_KEY_S)==GLFW_PRESS)
            y_offset-=20;
        if(glfwGetKey(WINDOW,GLFW_KEY_D)==GLFW_PRESS)
            x_offset+=20;

        e0.Move(x_offset,y_offset,DELTA_TIME*5);

        e0.Draw(*m_Renderer);
        e1.Draw(*m_Renderer);
        e2.Draw(*m_Renderer);
        e3.Draw(*m_Renderer);
        e4.Draw(*m_Renderer);
    }
};

int main(){
    Example *example=new Example("Test",1600,900);
    example->Run();
    delete example;

    return 0;
}