#include <pch.hpp>
#include <utilities.hpp>
#include <window.hpp>

float GetWidthPercentageInPx(float percentage){
    return percentage/100.0f*Window::Width;
}

float GetHeightPercentageInPx(float percentage){
    return percentage/100.0f*Window::Height;
}

bool StartNode(const std::string &name,enum ImGuiTreeNodeFlags_ flags){
    return ImGui::TreeNodeEx(name.c_str(),ImGuiTreeNodeFlags_SpanAvailWidth | flags);
}

std::string ExecuteCommand(const std::string &command){
    char buffer[128];
    std::string result;

    std::unique_ptr<FILE,decltype(&pclose)>pipe(popen(command.c_str(),"r"),pclose);
    if(!pipe){
        perror("popen failed");
    }
    while(fgets(buffer,128,pipe.get())!=nullptr) {
        result+=buffer;
    }
    return result;
} 