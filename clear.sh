#!/bin/bash

rm src/vendor/ImGui/libimgui_static.a -f
cd src
make clean
make config=release clean
rm ./Makefile -f
rm ./2DGameEngine.make -f
rm ./imgui.ini -f
rm bin/Debug/imgui.ini -f
rm bin/Release/imgui.ini -f

cd editor
make clean
make config=release clean
rm ./Makefile -f
rm ./2DGameEngineEditor.make -f
rm ./imgui.ini -f