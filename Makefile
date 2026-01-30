# Makefile for building C/C++ library as DLL (Windows) or SO (Linux/macOS)
# 
# Project structure:
#   include/DSA/      - Header files for DSA project (Pitchfork layout extra folder)
#   src/              - C and C++ source files
#   build/lib/        - Compiled libraries (.dll, .so, .dylib)
#   build/obj/        - Intermediate object files (.o)
#
# Usage:
#   make              - Build library
#   make clean        - Remove build artifacts
#   make help         - Show this message

# ============================================================================
# Detect OS and set target library name
# ============================================================================
ifeq ($(OS),Windows_NT)
    PLATFORM = Windows
    LIB_NAME = mylib.dll
    EXE_EXT = .exe
    CFLAGS = -fPIC -shared
else
    PLATFORM = $(shell uname -s)
    ifeq ($(PLATFORM),Linux)
        LIB_NAME = mylib.so
    else
        LIB_NAME = mylib.dylib
    endif
    EXE_EXT =
    CFLAGS = -fPIC -shared
endif

# ============================================================================
# Directories
# ============================================================================
SRC_DIR = src
TEST_DIR = test
INC_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
LIB_DIR = $(BUILD_DIR)/lib
TEST_BIN_DIR = $(BUILD_DIR)/test

# Build output path
TARGET = $(LIB_DIR)/$(LIB_NAME)

# Test executables - build all test_*.cpp files
TEST_SOURCES := $(wildcard $(TEST_DIR)/test_*.cpp)
TEST_EXES := $(patsubst $(TEST_DIR)/test_%.cpp,$(TEST_BIN_DIR)/test_%$(EXE_EXT),$(TEST_SOURCES))

# ============================================================================
# Compiler settings
# ============================================================================
CC = gcc
CXX = g++

# Compilation flags
CFLAGS_BASE = -Wall -Wextra -O2 -I. -I$(INC_DIR) $(CFLAGS)
CXXFLAGS_BASE = -Wall -Wextra -O2 -std=c++11 -I. -I$(INC_DIR) $(CFLAGS)

# ============================================================================
# Source files and objects
# ============================================================================
C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
CXX_SOURCES := $(filter-out $(SRC_DIR)/test_Ptr.cpp,$(wildcard $(SRC_DIR)/*.cpp))
# Collect public headers from the include layout
HEADERS := $(wildcard $(INC_DIR)/DSA/*.h)

# Map source files to object files in $(OBJ_DIR)
C_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
CXX_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CXX_SOURCES))
ALL_OBJECTS = $(C_OBJECTS) $(CXX_OBJECTS)

# ============================================================================
# Build rules
# ============================================================================
.PHONY: all clean help

all: $(TARGET) tests
	@echo "✓ Built $(TARGET) and test executables for $(PLATFORM)"

# Create output directories
$(LIB_DIR) $(OBJ_DIR) $(TEST_BIN_DIR):
	@mkdir -p $@

# Link final library (use C++ linker for C++ STL support)
$(TARGET): $(ALL_OBJECTS) | $(LIB_DIR)
	$(CXX) $(CXXFLAGS_BASE) -o $@ $^
	@echo "✓ Linking complete: $@"

# Pattern rule: compile each C source to corresponding object
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(OBJ_DIR)
	$(CC) -c $(CFLAGS_BASE) -o $@ $<
	@echo "✓ Compiled: $<"

# Pattern rule: compile each C++ source to corresponding object
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(OBJ_DIR)
	$(CXX) -c $(CXXFLAGS_BASE) -o $@ $<
	@echo "✓ Compiled: $<"

# Build test executables - one for each test_*.cpp file
tests: $(TEST_EXES)

$(TEST_BIN_DIR)/test_%$(EXE_EXT): $(TEST_DIR)/test_%.cpp $(SRC_DIR)/DSA.cpp $(SRC_DIR)/ClassRegistry.cpp | $(TEST_BIN_DIR)
	$(CXX) -Wall -Wextra -O2 -std=c++11 -I. -I$(INC_DIR) -o $@ $^
	@echo "✓ Built test executable: $@"

# Clean build artifacts
clean:
	@rm -rf $(BUILD_DIR)
	@echo "✓ Cleaned build artifacts"

# Show help
help:
	@echo "C/C++ Library Makefile"
	@echo "======================"
	@echo ""
	@echo "Usage:"
	@echo "  make, make all - Build the library ($(LIB_NAME)) and test executables"
	@echo "  make tests     - Build all test executables (only)"
	@echo "  make clean     - Remove build artifacts ($(BUILD_DIR)/)"
	@echo "  make help      - Show this message"
	@echo ""
	@echo "Platform: $(PLATFORM)"
	@echo "Target:   $(TARGET)"
	@echo ""
	@echo "Project structure:"
	@echo "  DSA/       - Header files by project (DSA, graph, network, etc.)"
	@echo "  $(SRC_DIR)/        - C and C++ source files"
	@echo "  $(TEST_DIR)/       - Test source files (test_*.cpp)"
	@echo "  $(LIB_DIR)/   - Compiled library"
	@echo "  $(TEST_BIN_DIR)/   - Compiled test executables"
	@echo "  $(OBJ_DIR)/   - Intermediate object files"
