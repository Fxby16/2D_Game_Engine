#include <pch.hpp>
#include <input.hpp>
#include <window.hpp>

void Input::UpdateStates(){
    for(auto &key:m_KeyStates){
        key.second.previous=key.second.current;
        key.second.current=glfwGetKey(Window::Window,key.first);
    }

    for(auto &button:m_MouseStates){
        button.second.previous=button.second.current;
        button.second.current=glfwGetMouseButton(Window::Window,button.first);
    }
}

Vec2 Input::GetMousePosition(){
    double x,y;
    glfwGetCursorPos(Window::Window,&x,&y);
    return Vec2(x/Window::Width*Window::MAX_WIDTH,(Window::Height-y)/Window::Height*Window::MAX_HEIGHT);   
}