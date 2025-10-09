.PHONY: all clean

PROJECT_NAME ?= game

COMPILER_PATH = $(COMPILER_PATH)/bin
CC = g++

LIBS = $(shell scripts/read_list_file.sh "./config/libs.txt" "-l")
LIBS_FOLDERS = $(shell scripts/read_list_file.sh "./config/lib_source.txt" "-L")
INC_FOLDERS= $(shell scripts/read_list_file.sh "./config/lib_include.txt" "-I")
SOURCE_INC = $(shell scripts/read_list_file.sh "./config/source_include.txt")
SOURCE_EXC = $(shell scripts/read_list_file.sh "./config/source_exclude.txt")
FLAGS = $(shell scripts/read_list_file.sh "./config/flags.txt")

SOURCE_FILES = $(shell find $(SOURCE_INC) -maxdepth 1 -type f -name "*.c" -or -name "*.cpp")# -not -name ${SOURCE_EXCLUDE}")

all: build

build:
	$(CC) $(SOURCE_FILES) -o $(PROJECT_NAME).exe $(FLAGS) $(LIBS_FOLDERS) $(INC_FOLDERS) $(LIBS)
clean:
	rm -f *.o
run:
	./$(PROJECT_NAME).exe
