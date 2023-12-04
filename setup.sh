cd src/vendor/FreeType
make setup ansi
make

cd ..
cd glfw
cmake -S . -B build
cd build
make

cd ../../genie
make
cd ../soloud/build
../../genie/bin/linux/genie gmake
cd gmake
make

cd ../../../../..
bash ./imgui.sh