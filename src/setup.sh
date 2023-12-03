#!/bin/bash

# Directory dei file sorgente di ImGui
imgui_src_dir="vendor/ImGui"

# Nome della libreria statica
lib_name="libimgui_static.a"

# Compila tutti i file .cpp nella directory di ImGui
objects=()
for file in "${imgui_src_dir}"/*.cpp; do
    base_name=$(basename "$file")
    object_name="${base_name%.cpp}.o"
    g++ -c -o "${object_name}" "${file}" -I "${imgui_src_dir}"
    objects+=("${object_name}")
done

# Compila i file specifici di imgui_impl_glfw e imgui_impl_opengl3
g++ -c -o ./imgui_impl_glfw.o "${imgui_src_dir}/backends/imgui_impl_glfw.cpp" -I "${imgui_src_dir}" -I "${imgui_src_dir}/backends"
g++ -c -o ./imgui_impl_opengl3.o "${imgui_src_dir}/backends/imgui_impl_opengl3.cpp" -I "${imgui_src_dir}" -I "${imgui_src_dir}/backends"

# Aggiungi i file oggetto specifici alla lista degli oggetti
objects+=("imgui_impl_glfw.o")
objects+=("imgui_impl_opengl3.o")

# Crea l'archivio (libreria statica)
ar rcs "${lib_name}" "${objects[@]}"

# Copia la libreria statica nel tuo progetto
cp "${lib_name}" "${imgui_src_dir}/${lib_name}"

# Pulisci i file oggetto temporanei
rm "${objects[@]}"
# Aggiungi altri comandi di pulizia se necessario

rm "${lib_name}"