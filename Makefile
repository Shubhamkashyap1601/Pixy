CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -I./src -mconsole
SRC = $(wildcard src/**/*.cpp) src/main.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = pixy

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)