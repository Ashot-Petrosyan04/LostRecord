CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = tests
COMPILER = lostrecordc
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
CPPFLAGS = -I$(SRC_DIR)

all: $(COMPILER)

$(COMPILER): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(COMPILER) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

run: $(COMPILER)
	@if [ -z "$(SOURCE)" ]; then \
		echo "Usage: make run SOURCE=<your_file.lr>"; \
		exit 1; \
	fi
	@echo "--- Compiling $(TEST_DIR)/$(SOURCE) with $(COMPILER) ---"
	./$(COMPILER) $(TEST_DIR)/$(SOURCE) > output.s
	@echo "--- Assembling output.s with nasm ---"
	nasm -f elf64 output.s -o output.o
	@echo "--- Linking output.o with ld ---"
	ld output.o -o program
	@echo "--- Running program ---"
	./program
	@echo "--- Done ---"

clean:
	rm -f $(COMPILER) $(OBJ_DIR)/*.o output.s output.o program
	@rmdir $(OBJ_DIR) 2>/dev/null || true

.PHONY: all clean run