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
