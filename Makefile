TARGET = nezabudka
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -g -MMD -MP
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
INCLUDES = -Ilibs   # библиотеки с заголовками

all: directories $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

directories:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

run: all
	@./$(BIN_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

-include $(OBJECTS:.o=.d)
.PHONY: all clean run directories
