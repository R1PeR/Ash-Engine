.PHONY: all clean

PROJECT_NAME ?= game

COMPILER_PATH = C:/w64devkit/bin
CC = g++

LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
LIB_SRC = -Llibs/raylib/src
INCLUDE = -Ilibs/raylib/src -Ilibs/imgui -Ilibs/rlImGui -Ilibs -I.
SOURCE_FOLDERS = ./ modes/ libs/engine/components libs/engine/misc/ libs/engine/context/ libs/engine/io libs/rlImGui/ libs/imgui/
SOURCE_FILES = $(shell find $(SOURCE_FOLDERS) -maxdepth 1 -type f -name "*.c" -or -name "*.cpp")
FLAGS = -Wall -Wextra -std=c++11 -g -DSUPPORT_TRACELOG -DDEBUG -DLOG_LEVEL=LOG_LEVEL_INFO

all: build

build:
	$(COMPILER_PATH)/$(CC) $(SOURCE_FILES) -o $(PROJECT_NAME).exe $(FLAGS) $(LIB_SRC) $(INCLUDE) $(LIBS)
clean:
	rm -f *.o
run:
	./$(PROJECT_NAME).exe