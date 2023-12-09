#!/bin/bash

rm src/vendor/ImGui/libimgui_static.a -f
cd src
make clean
rm ./Makefile -f
rm ./2DGameEngine.make -f
rm ./imgui.ini -f

cd editor
make clean
rm ./Makefile -f
rm ./2DGameEngineEditor.make -f