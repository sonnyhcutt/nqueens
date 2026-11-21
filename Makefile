# Simple build for systems without CMake. See README for the CMake build.

CXX      ?= c++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
CPPFLAGS += -Iinclude

BUILD_DIR := build
CORE_SRCS := src/board_io.cpp src/input.cpp src/knights_tour.cpp src/nqueens.cpp \
             src/nqueens_solvers.cpp
CORE_OBJS := $(CORE_SRCS:%.cpp=$(BUILD_DIR)/%.o)

APP   := $(BUILD_DIR)/puzzles
TESTS := $(BUILD_DIR)/puzzles_tests
BENCH := $(BUILD_DIR)/nqueens_bench

# Extra arguments for `make bench`, e.g. make bench BENCH_ARGS="--max-n 60"
BENCH_ARGS ?= --csv $(BUILD_DIR)/nqueens_bench.csv

.PHONY: all run test bench clean

all: $(APP) $(BENCH)

$(APP): $(CORE_OBJS) $(BUILD_DIR)/src/main.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TESTS): $(CORE_OBJS) $(BUILD_DIR)/tests/test_main.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BENCH): $(CORE_OBJS) $(BUILD_DIR)/bench/nqueens_bench.o
	$(CXX) $(CXXFLAGS) $^ -o $@

bench: $(BENCH)
	./$(BENCH) $(BENCH_ARGS)

$(BUILD_DIR)/%.o: %.cpp $(wildcard include/*.h)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

run: $(APP)
	./$(APP)

test: $(TESTS)
	./$(TESTS)

clean:
	rm -rf $(BUILD_DIR)
