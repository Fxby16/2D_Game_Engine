workspace "Application"
    configurations { "Debug", "Release" }

project "Application"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"
    location "temp"

    files { "**.hpp", "**.cpp", "**.h", "**.c",
            "../include/**.hpp", "../src/**.cpp", "../include/**.h", "../src/**.c",
            "../vendor/glad/src/glad.c" }
    removefiles { "vendor/**", "src/entrypoint.cpp", "src/application_template.cpp", "src/editor.cpp" }
    files { "temp/finalapplication.cpp" }

    pchheader "pch.hpp"

    includedirs { "include",
                  "../include",
                  "../vendor/glm",
                  "../vendor/glfw/include/GLFW",
                  "../vendor/ImGui",
                  "../vendor/ImGui/backends",
                  "../vendor/glad/include",
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
            "freetype", 
            "soloud_static",
            "asound",
            "box2d",
            "yaml-cpp" }

    filter "configurations:Debug"
        optimize "Debug"
        defines { "DEBUG", "APPLICATION" }
        symbols "On"

    filter "configurations:Release"
        optimize "Full"
        defines { "RELEASE", "APPLICATION" }
        symbols "Off"
        