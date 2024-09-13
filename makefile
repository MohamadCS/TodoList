COMPILER=g++
VERSION = -std=c++2b
CFLAGS=
INCLUDE_DIR=./include/TodoList
SRC_DIR = ./src
BUILD_DIR = ./build
LINK = $(shell ~/Developer/CppLibraries/wxWidgets/macbuild/wx-config --cxxflags --libs) # change for general use
CC= $(COMPILER) $(VERSION)
TARGET=program


all: clean build

leaks: build
	   leaks -atExit -- ./$(TARGET)

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LINK)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE_DIR) -c $< -o $@ $(LINK)

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/program

.PHONY: all clean
