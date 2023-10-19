DEBUG ?= 1
ifeq ($(DEBUG),1)
	CFLAGS = -Wall
else
	CFLAGS = -Ofast
endif

CC := g++
CINCLUDES := -I include/ -I include/vendor/imgui/ -I include/vendor
CLIBRARIES := -L lib/glfw -lglfw3 -lX11 -lpthread -ldl
TARGET := main

SRCS := $(wildcard *.cpp src/vendor/imgui/*.cpp src/vendor/*.c src/*.cpp)

all: $(TARGET)
$(TARGET): $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(CINCLUDES) $(CLIBRARIES)
