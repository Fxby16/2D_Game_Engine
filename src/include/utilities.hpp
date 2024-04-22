#pragma once

extern float GetWidthPercentageInPx(float percentage);
extern float GetHeightPercentageInPx(float percentage);
extern bool StartNode(const std::string &name,enum ImGuiTreeNodeFlags_ flags=ImGuiTreeNodeFlags_None);

extern std::string ExecuteCommand(const std::string &command);

extern std::string ExtractFileName(const std::string &path);