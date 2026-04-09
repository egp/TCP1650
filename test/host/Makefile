CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror -pedantic -Iinclude
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)
BUILD_DIR := build/$(UNAME_S)-$(UNAME_M)
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
