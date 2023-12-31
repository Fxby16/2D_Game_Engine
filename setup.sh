#!/bin/bash

apt install cmake
apt install libasound2-dev
apt install xorg-dev
apt install libglu1-mesa-dev freeglut3-dev mesa-common-dev
apt install zenity

# Setup GLFW
cd src/vendor/glfw
cmake -S . -B build
cd build
make

cd ../../../../

# Setup ImGui
if ! [ -e src/vendor/ImGui/libimgui_static.a ]; then
    imgui_src_dir="src/vendor/ImGui" 
    lib_name="libimgui_static.a"

    objects=()
    for file in "${imgui_src_dir}"/*.cpp; do
        base_name=$(basename "$file")
        object_name="${base_name%.cpp}.o"
        g++ -c -o "${object_name}" "${file}" -I "${imgui_src_dir}" -I src/vendor/glfw
        objects+=("${object_name}")
    done

    pwd
    g++ -c -o ./imgui_impl_glfw.o "${imgui_src_dir}/backends/imgui_impl_glfw.cpp" -I "${imgui_src_dir}" -I "${imgui_src_dir}/backends" -I src/vendor/glfw/include
    g++ -c -o ./imgui_impl_opengl3.o "${imgui_src_dir}/backends/imgui_impl_opengl3.cpp" -I "${imgui_src_dir}" -I "${imgui_src_dir}/backends" -I src/vendor/glfw/include

    objects+=("imgui_impl_glfw.o")
    objects+=("imgui_impl_opengl3.o")

    ar rcs "${lib_name}" "${objects[@]}"
    cp "${lib_name}" "${imgui_src_dir}/${lib_name}"
    rm "${objects[@]}"
    rm "${lib_name}"
fi

# Setup FreeType
cd src/vendor/FreeType
make setup ansi
make
pwd

# Setup Genie
cd ../genie
make

# Setup Soloud
cd ../soloud/build
../../genie/bin/linux/genie gmake
cd gmake
make

cd ../../../..

# Setup NativeFileDialog
if ! [ -z "$1" ] && [ $1==1 ]; then
    cd editor/vendor/NativeFileDialog/build/gmake_linux_zenity
    make 
    cd ../../../../../
fi

#setup box2d
cd vendor/box2d
bash ./build.sh