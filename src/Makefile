DEBUG ?= 1
ifeq ($(DEBUG),1)
   CFLAGS = -Wall
else
	CFLAGS = -Ofast
endif

CC := g++
CINCLUDES := -I include/ -I include/vendor/imgui/ -I include/vendor
CLIBRARIES := -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl
TARGET := main

SRCS := $(wildcard *.cpp src/vendor/imgui/*.cpp src/vendor/*.c src/*.cpp)

all: $(TARGET)
$(TARGET): $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(CINCLUDES) $(CLIBRARIES)
