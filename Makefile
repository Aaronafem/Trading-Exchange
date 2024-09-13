# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

# Libraries
LDFLAGS = -lncurses

# Source files
SRC = main.cpp

# Output executable
OUT = orderbook

# Default target
all: $(OUT)

# Compile the program
$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)

# Clean target to remove compiled files
clean:
	rm -f $(OUT)


# Phony targets
.PHONY: all clean
