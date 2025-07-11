CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -I./src
SRC = $(wildcard src/**/*.cpp) src/main.cpp
SRC_UI = $(wildcard src/**/*.cpp) src/main_ui.cpp
OBJ = $(SRC:.cpp=.o)
OBJ_UI = $(SRC_UI:.cpp=.o)
TARGET = pixy
TARGET_UI = pixy-ui

all: $(TARGET) $(TARGET_UI)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

$(TARGET_UI): $(OBJ_UI)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ_UI)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(TARGET_UI) $(OBJ) $(OBJ_UI)