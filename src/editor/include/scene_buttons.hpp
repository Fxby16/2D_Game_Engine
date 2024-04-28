#pragma once

#include <scene.hpp>

class SceneButtons{
public:
    /**
     * Called when play button is pressed in the editor.
     * 
     * Generates the application source code, compiles and runs the application in another thread. If the application is already running, it does nothing.
    */
    static void PlayButton(const std::string &path,const std::string &editor_path,std::vector<std::string>&scene_names,std::vector<std::vector<std::pair<std::string,uint32_t>>>&script_components);
};