workspace "2DGameEngine"
    configurations { "Debug", "Release" }

project "2DGameEngine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"

    files { "**.hpp", "**.cpp", "**.h", "**.c" }
    removefiles { "examples/**" }

    includedirs { "include", 
                  "include/vendor", 
                  "include/vendor/imgui/", 
                  "include/vendor/freetype2", 
                  "include/vendor/soloud" }

    libdirs { "lib" }

    links { "freetype", 
            "glfw3", 
            "soloud_static",
            "X11",
            "pthread",
            "dl",
            "z",
            "asound" }

    filter "configurations:Debug"
        optimize "Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "Full"
        