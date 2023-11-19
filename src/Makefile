DEBUG ?= 1
ifeq ($(DEBUG),1)
	CFLAGS = -Wall -g -ggdb -Og
else
	CFLAGS = -Ofast
endif

CC := g++
CINCLUDES := -I include/ -I include/vendor/imgui/ -I include/vendor -I include/vendor/freetype2 -I include/vendor/soloud
CLIBRARIES := -L lib/ -lglfw3 -lX11 -lpthread -ldl -lfreetype -lz -lsoloud_static -lasound
TARGET := main

SRCS := $(wildcard *.cpp src/vendor/imgui/*.cpp src/vendor/*.c src/*.cpp)

all: $(TARGET)
$(TARGET): $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(CINCLUDES) $(CLIBRARIES)
