#include <utilities.hpp>
#include <window.hpp>

float GetWidthPercentageInPx(float percentage){
    return percentage/100.0f*Window::Width;
}

float GetHeightPercentageInPx(float percentage){
    return percentage/100.0f*Window::Height;
}