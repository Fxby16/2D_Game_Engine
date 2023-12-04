#!/bin/bash

imgui_src_dir="src/vendor/ImGui"
lib_name="libimgui_static.a"

objects=()
for file in "${imgui_src_dir}"/*.cpp; do
    base_name=$(basename "$file")
    object_name="${base_name%.cpp}.o"
    g++ -c -o "${object_name}" "${file}" -I "${imgui_src_dir}"
    objects+=("${object_name}")
done

g++ -c -o ./imgui_impl_glfw.o "${imgui_src_dir}/backends/imgui_impl_glfw.cpp" -I "${imgui_src_dir}" -I "${imgui_src_dir}/backends"
g++ -c -o ./imgui_impl_opengl3.o "${imgui_src_dir}/backends/imgui_impl_opengl3.cpp" -I "${imgui_src_dir}" -I "${imgui_src_dir}/backends"

objects+=("imgui_impl_glfw.o")
objects+=("imgui_impl_opengl3.o")

ar rcs "${lib_name}" "${objects[@]}"

cp "${lib_name}" "${imgui_src_dir}/${lib_name}"

rm "${objects[@]}"

rm "${lib_name}"