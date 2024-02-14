# 2D Game Engine
A work-in-progress 2D game engine made in c++ and opengl.

## Clone
Run `git clone --recursive https://github.com/Fxby16/2D_Game_Engine.git`

## Requirements
Run `sudo bash setup.sh`. If you want to run the editor run `sudo bash setup.sh 1`

## Build
`cd src`  
`premake5 gmake2`  
`make`  
If not specified, the program will be compiled in debug mode. To compile in release mode use `make config=release`.

If you want to build the editor:
`cd src/editor`
`premake5 gmake2`
`make`

## Execute
Run `bin/Debug/2DGameEngine` or `bin/Release/2DGameEngine` depending on which config you used.
To run the editor run `bin/Debug/2DGameEngineEditor` or `bin/Release/2DGameEngineEditor` (in src/editor)

## Documentation
You can find the documentation [here](https://fxby16.github.io/2D_Game_Engine/).  
**Note:** the documentation might not be updated