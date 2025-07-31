CXX = g++
CXXFLAGS_DEBUG = -std=c++17 -Wall -g -Isrc
CXXFLAGS_RELEASE = -std=c++17 -Wall -O3 -Isrc
LDFLAGS =

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

TARGET = lostrecordc

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS_DEBUG = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/debug/%.o, $(SRCS))
OBJS_RELEASE = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/release/%.o, $(SRCS))

all: release

debug: $(BIN_DIR)/$(TARGET)_debug

$(BIN_DIR)/$(TARGET)_debug: $(OBJS_DEBUG)
	@mkdir -p $(BIN_DIR)
	@echo "Linking debug build..."
	@$(CXX) $(CXXFLAGS_DEBUG) -o $@ $^ $(LDFLAGS)
	@echo "Debug build finished: $@"

$(OBJ_DIR)/debug/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)/debug
	@echo "Compiling $< (debug)..."
	@$(CXX) $(CXXFLAGS_DEBUG) -c $< -o $@

release: $(BIN_DIR)/$(TARGET)_release

$(BIN_DIR)/$(TARGET)_release: $(OBJS_RELEASE)
	@mkdir -p $(BIN_DIR)
	@echo "Linking release build..."
	@$(CXX) $(CXXFLAGS_RELEASE) -o $@ $^ $(LDFLAGS)
	@echo "Release build finished: $@"

$(OBJ_DIR)/release/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)/release
	@echo "Compiling $< (release)..."
	@$(CXX) $(CXXFLAGS_RELEASE) -c $< -o $@

run: release
	@if [ -z "$(SOURCE)" ]; then \
		echo "Usage: make run SOURCE=<your_file.lr>"; \
		exit 1; \
	fi
	./$(BIN_DIR)/$(TARGET)_release tests/$(SOURCE) > output.s
	nasm -f elf64 output.s -o output.o
	ld output.o -o program
	./program

clean:
	@echo "Cleaning up..."
	@rm -rf $(OBJ_DIR) $(BIN_DIR) $(TARGET) output.s output.o program
	@echo "Cleanup complete."

.PHONY: all debug release run clean
