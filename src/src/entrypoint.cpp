#include <pch.hpp>
#include <window.hpp>
#include <application.hpp>

int main(int argc,char **argv){
    auto application=Window::CreateApplication();
    application->Run();
    delete application;

    Window::DeinitGlfwWindow();

    return 0;
}