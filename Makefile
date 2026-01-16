CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -I/opt/homebrew/include -Isrc
LDFLAGS = -L/opt/homebrew/lib -lSDL2

TARGET = macGame
SOURCES = src/main.cpp src/tetris.cpp src/graphics.cpp src/input.cpp
HEADERS = src/tetris.hpp src/constants.hpp

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
