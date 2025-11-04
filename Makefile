# --- Project Setup ---
PROJECT_NAME ?= game
EXECUTABLE = $(PROJECT_NAME).exe

# --- Makefile Settings ---
MAKEFLAGS := -j 8

# --- Compiler Setup ---
CC = g++ -pipe

# --- Directories ---
# Create a dedicated directory for object files
OBJ_DIR = obj

# --- Read Config Files (Using := for immediate expansion) ---
# [cite: 5]
LIBS         := $(shell scripts/read_list_file.sh "./config/libs.txt" "-l")
# [cite: 3]
LIBS_FOLDERS := $(shell scripts/read_list_file.sh "./config/lib_source.txt" "-L")
# [cite: 6]
INC_FOLDERS  := $(shell scripts/read_list_file.sh "./config/lib_include.txt" "-I")
# [cite: 2]
SOURCE_DIRS  := $(shell scripts/read_list_file.sh "./config/source_include.txt")
# [cite: 4]
FLAGS        := $(shell scripts/read_list_file.sh "./config/flags.txt")

# --- Find Source Files (Recursively) ---
# 1. Find all .c and .cpp files recursively in your SOURCE_DIRS [cite: 2]
RAW_SOURCES := $(shell find $(SOURCE_DIRS) -maxdepth 1 -type f -name "*.c" -or -name "*.cpp")

# 2. Read the raw list of directories to exclude 
RAW_EXCLUDE_DIRS := $(shell scripts/read_list_file.sh "./config/source_exclude.txt")

# --- Normalize and Filter (The Fix) ---
# 3. Clean both lists by removing './' and extra whitespace
CLEAN_SOURCES      := $(patsubst ./%,%, $(strip $(RAW_SOURCES)))
CLEAN_EXCLUDE_DIRS := $(patsubst ./%,%, $(strip $(RAW_EXCLUDE_DIRS)))

# 4. Create wildcard patterns from the exclude directories
#    'libs/imgui/backends' becomes 'libs/imgui/backends/%'
EXCLUDE_PATTERNS := $(addsuffix /**, $(CLEAN_EXCLUDE_DIRS))

# 5. Filter the source list. 'filter-out' will now correctly remove
#    any file matching 'libs/imgui/backends/%' or 'oldlibs/%' 
SOURCE_FILES := $(filter-out $(EXCLUDE_PATTERNS), $(CLEAN_SOURCES))

# --- Create Object File List (Preserving Directory Structure) ---
# This turns 'src/main.cpp' into 'obj/src/main.o'
OBJ_FILES := $(SOURCE_FILES)
OBJ_FILES := $(OBJ_FILES:%.cpp=%.o)
OBJ_FILES := $(OBJ_FILES:%.c=%.o)
OBJ_FILES := $(addprefix $(OBJ_DIR)/, $(OBJ_FILES))

# --- Targets ---
.PHONY: all build run clean

# Default target
all: build

# 'build' is now an alias for the executable file
build: $(EXECUTABLE)

# --- Linking Rule ---
# Links all object files ($^) into the final executable ($@)
$(EXECUTABLE): $(OBJ_FILES)
	@echo "Linking $@..."
	@$(CC) $^ -o $@ $(FLAGS) $(LIBS_FOLDERS) $(LIBS)

# --- Compilation Pattern Rule (for .cpp files) ---
# Compiles a single source file ($<) into an object file ($@)
$(OBJ_DIR)/%.o: %.cpp
	@echo "Compiling $@..."
	@mkdir -p $(dir $@)
	@$(CC) -c $< -o $@ $(FLAGS) $(INC_FOLDERS)

# --- Compilation Pattern Rule (for .c files) ---
$(OBJ_DIR)/%.o: %.c
	@echo "Compiling $@..."
	@mkdir -p $(dir $@)
	@$(CC) -c $< -o $@ $(FLAGS) $(INC_FOLDERS)

# --- Utility Targets ---
run: build
	./$(EXECUTABLE)

clean:
	@echo "Cleaning..."
	@rm -f $(EXECUTABLE)
	@rm -rf $(OBJ_DIR)
