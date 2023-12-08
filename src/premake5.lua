workspace "2DGameEngine"
    configurations { "Debug", "Release" }

project "2DGameEngine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"

    files { "**.hpp", "**.cpp", "**.h", "**.c" }
    removefiles { "examples/**","vendor/**","editor/**" }

    includedirs { "include", 
                  "include/vendor", 
                  "vendor/FreeType/include",
                  "vendor/glfw/include/GLFW",
                  "vendor/ImGui",
                  "vendor/ImGui/backends",
                  "vendor/soloud/include"}

    libdirs { "vendor/FreeType/objs",
              "vendor/glfw/build/src",
              "vendor/ImGui",
              "vendor/soloud/lib" }

    links { "freetype", 
            "glfw3", 
            "imgui_static",
            "soloud_static",
            "X11",
            "pthread",
            "dl",
            "z",
            "asound" }

    filter "configurations:Debug"
        optimize "Debug"
        symbols "On"
        defines { "DEBUG" }

    filter "configurations:Release"
        optimize "Full"
        