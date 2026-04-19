CXX ?= g++
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror -pedantic -Itest/host/include -Isrc

BUILD_DIR := build/$(UNAME_S)-$(UNAME_M)
HOST_BIN := $(BUILD_DIR)/host_tests
HOST_SOURCES := $(wildcard test/host/src/*.cpp) src/TCP1650_Device.cpp src/TCP1650_Font.cpp

ARDUINO_CLI ?= arduino-cli
TCP1819_DIR ?= ../TCP1819
FQBN ?= arduino:avr:uno
EXAMPLE_DIRS := $(sort $(dir $(wildcard examples/*/*.ino)))

.PHONY: all test host-tests compile-examples ci clean

all: test

test: host-tests compile-examples

host-tests: $(HOST_BIN)
	$(HOST_BIN)

$(HOST_BIN): $(HOST_SOURCES)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(HOST_SOURCES) -o $(HOST_BIN)

compile-examples:
	@if ! command -v "$(ARDUINO_CLI)" >/dev/null 2>&1; then \
		echo "error: $(ARDUINO_CLI) not found in PATH"; \
		exit 1; \
	fi
	@if [ ! -d "$(TCP1819_DIR)" ]; then \
		echo "error: TCP1819_DIR='$(TCP1819_DIR)' not found"; \
		echo "clone egp/TCP1819 alongside this repo or override TCP1819_DIR=/path/to/TCP1819"; \
		exit 1; \
	fi
	@set -e; \
	for example in $(EXAMPLE_DIRS); do \
		echo "==> Compiling $$example for $(FQBN)"; \
		"$(ARDUINO_CLI)" compile \
			--fqbn "$(FQBN)" \
			--warnings all \
			--library "$(CURDIR)" \
			--library "$(abspath $(TCP1819_DIR))" \
			"$$example"; \
	done

ci: test

clean:
	rm -rf build