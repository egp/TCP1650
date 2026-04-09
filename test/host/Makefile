CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror -pedantic -Iinclude
BUILD_DIR := build
BIN := $(BUILD_DIR)/host_tests

SOURCES := $(wildcard src/*.cpp)

.PHONY: all test clean

all: test

test: $(BIN)
	$(BIN)

$(BIN): $(SOURCES)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(BIN)

clean:
	rm -rf $(BUILD_DIR)
