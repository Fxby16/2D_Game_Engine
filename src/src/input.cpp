#include <pch.hpp>
#include <input.hpp>
#include <window.hpp>

void Input::UpdateStates(){
    for(auto &key:m_KeyStates){
        key.second.previous=key.second.current;
        key.second.current=glfwGetKey(Window::Window,key.first);
    }
}