workspace "2DGameEngine"
    configurations { "Debug", "Release" }

project "2DGameEngine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"

    files { "**.hpp", "**.cpp", "**.h", "**.c" }
    removefiles { "examples/**","vendor/**","editor/**" }
    files { "vendor/glad/src/glad.c" }

    includedirs { "include", 
                  "include/vendor", 
                  "vendor/instrumentor",
                  "vendor/glad/include",
                  "vendor/stb_image",
                  "vendor/FreeType/include",
                  "vendor/glfw/include/GLFW",
                  "vendor/ImGui",
                  "vendor/ImGui/backends",
                  "vendor/soloud/include",
                  "vendor/box2d/include/" }

    libdirs { "vendor/FreeType/objs",
              "vendor/glfw/build/src",
              "vendor/ImGui",
              "vendor/soloud/lib",
              "vendor/box2d/build/bin" }

    links { "freetype", 
            "glfw3", 
            "imgui_static",
            "soloud_static",
            "X11",
            "pthread",
            "dl",
            "z",
            "asound",
            "box2d" }   

    filter "configurations:Debug"
        optimize "Debug"
        symbols "On"
        defines { "DEBUG" }

    filter "configurations:Release"
        optimize "Full"

    filter "configurations:Profile"
        optimize "Full"
        defines { "ENABLE_PROFILING" }
        