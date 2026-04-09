CXX ?= g++
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror -pedantic -Iinclude -I../../src
BUILD_DIR := build/$(UNAME_S)-$(UNAME_M)
BIN := $(BUILD_DIR)/host_tests

SOURCES := $(wildcard src/*.cpp) ../../src/TCP1650_Device.cpp ../../src/TCP1650_Font.cpp

.PHONY: all test clean

all: test

test: $(BIN)
	$(BIN)

$(BIN): $(SOURCES)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(BIN)

clean:
	rm -rf build