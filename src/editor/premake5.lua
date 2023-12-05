workspace "2DGameEngineEditor"
    configurations { "Debug", "Release" }

project "2DGameEngineEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    files { "**.hpp", "**.cpp", "**.h", "**.c" }
    removefiles { "vendor/NativeFileDialog/**" }

    includedirs { "include",
                  "../include/vendor", 
                  "../vendor/FreeType/include",
                  "../vendor/glfw/include/GLFW",
                  "../vendor/ImGui",
                  "../vendor/ImGui/backends",
                  "../vendor/soloud/include",
                  "vendor/NativeFileDialog/src/include",
                  "vendor/NativeFileDialog/src/"}

    libdirs { "../vendor/glfw/build/src",
              "../vendor/ImGui",
              "vendor/NativeFileDialog/build/lib/Release/x64/" }

    links { "glfw3", 
            "imgui_static",
            "X11",
            "pthread",
            "dl",
            "z",
            "nfd" }

    filter "configurations:Debug"
        optimize "Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "Full"
        