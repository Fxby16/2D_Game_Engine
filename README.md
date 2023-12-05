# 2D Game Engine
A work-in-progress 2D game engine made in c++ and opengl.

## Clone
Run `git clone --recursive https://github.com/Fxby16/2D_Game_Engine.git`

## Requirements
`sudo apt install xorg-dev`  
`sudo apt install libglu1-mesa-dev freeglut3-dev mesa-common-dev`  
Required only for the editor (there isn't one yet...):  
`sudo apt install zenity`

## Build
`bash setup.sh`  
`cd src`  
`premake5 gmake2`  
`make`  
To get the executable. If not specified, the program will be compiled in debug mode. To compile in release mode use `make config=release`.

## Execute
Run `bin/Debug/2DGameEngine` or `bin/Release/2DGameEngine` depending on which config you used.

## Short term plans
- start to write a documentation
- make the code more readable and more organized
- add other features

## Long term plans
- add an editor
- windows porting
