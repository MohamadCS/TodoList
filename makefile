COMPILER=g++
VERSION = -std=c++2b
CFLAGS=
INCLUDE_DIR=./include/TodoList
SRC_DIR = ./src
BUILD_DIR = ./build
WX_WIDGETS_LINK = $(shell ~/Developer/CppLibraries/wxWidgets/macbuild/wx-config --cxxflags --libs)
CC= $(COMPILER) $(VERSION)
TARGET=TodoList

# test
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

all: clean $(BUILD_DIR)/$(TARGET)

run: clean $(BUILD_DIR)/$(TARGET) 
	$(BUILD_DIR)/$(TARGET)

leaks: $(BUILD_DIR)/$(TARGET) 
	leaks -atExit -- $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(WX_WIDGETS_LINK)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE_DIR) -c $< -o $@ $(WX_WIDGETS_LINK) 

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/$(TARGET)

.PHONY: all clean
