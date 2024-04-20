workspace "2DGameEngineEditor"
    configurations { "Debug", "Release", "Profile" }

project "2DGameEngineEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"

    files { "include/**.hpp", "src/**.cpp", "include/**.h", "src/**.c",
            "../include/**.hpp", "../src/**.cpp", "../include/**.h", "../src/**.c",
            "../vendor/glad/src/glad.c" }
    removefiles { "vendor/**", "../src/entrypoint.cpp", "../src/application.cpp", "src/application_template.cpp" }

    pchheader "pch.hpp"

    includedirs { "include",
                  "../include",
                  "../vendor/glm",
                  "../vendor/glfw/include/GLFW",
                  "../vendor/ImGui",
                  "../vendor/ImGui/backends",
                  "../vendor/glad/include",
                  "vendor/NativeFileDialog/src/include",
                  "vendor/NativeFileDialog/src/",
                  "vendor/FontAwesome",
                  "../include/vendor", 
                  "../vendor/instrumentor",
                  "../vendor/glad/include",
                  "../vendor/stb_image",
                  "../vendor/FreeType/include",
                  "../vendor/soloud/include",
                  "../vendor/box2d/include/",
                  "vendor/yaml-cpp/include" }

    libdirs { "../vendor/glfw/build/src",
              "../vendor/ImGui",
              "vendor/NativeFileDialog/build/lib/Release/x64/",
              "../vendor/FreeType/objs",
              "../vendor/soloud/lib",
              "../vendor/box2d/build/bin",
              "vendor/yaml-cpp/build" }

    links { "glfw3", 
            "imgui_static",
            "X11",
            "pthread",
            "dl",
            "z",
            "nfd",
            "freetype", 
            "soloud_static",
            "asound",
            "box2d",
            "yaml-cpp" }

    filter "configurations:Debug"
        optimize "Debug"
        defines { "DEBUG", "EDITOR" }
        symbols "On"

    filter "configurations:Release"
        optimize "Full"
        symbols "Off"
        defines { "RELEASE", "EDITOR" }
    
    filter "configurations:Profile"
        optimize "Full"
        defines { "ENABLE_PROFILING", "EDITOR" }