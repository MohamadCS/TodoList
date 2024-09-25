COMPILER=g++
VERSION = -std=c++2b
CFLAGS=
INCLUDE_DIR=./include/TodoList
SRC_DIR = ./src
SQLITE_DIR=./include/sqlite
SQLITE_OBJ=$(SQLITE_DIR)/sqlite.o
BUILD_DIR = ./build
WX_WIDGETS_LINK = $(shell ~/Developer/CppLibraries/wxWidgets/macbuild/wx-config --cxxflags --libs)
CXX= $(COMPILER) $(VERSION)
CC=gcc
TARGET=TodoList

# Gather C and C++ source files
C_SOURCES = $(wildcard $(SRC_DIR)/*.c)
CPP_SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
SOURCES = $(C_SOURCES) $(CPP_SOURCES)

# Object files for both C and C++
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES)) $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CPP_SOURCES))

all: clean $(BUILD_DIR)/$(TARGET)

run: clean $(BUILD_DIR)/$(TARGET) 
	$(BUILD_DIR)/$(TARGET)

leaks: $(BUILD_DIR)/$(TARGET) 
	leaks -atExit -- $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) $(SQLITE_OBJ) -o $@ $(WX_WIDGETS_LINK)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CXX) $(CFLAGS) -c $< -o $@ $(WX_WIDGETS_LINK) 

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) $(INCLUDE_DIR) -c $< -o $@ $(WX_WIDGETS_LINK)

sqlite: $(SQLITE_DIR)/sqlite3.c
		$(CC) -c $(SQLITE_DIR)/sqlite3.c -o $(SQLITE_DIR)/sqlite.o

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/$(TARGET)

.PHONY: all clean run leaks

