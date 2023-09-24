CC := g++
CFLAGS := -Wall -Ofast
CINCLUDES := -I include/ -I include/imgui/
CLIBRARIES := -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl
TARGET := main

SRCS := $(wildcard *.cpp src/*.c* src/imgui/*.cpp)

all: $(TARGET)
$(TARGET): $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(CINCLUDES) $(CLIBRARIES)
