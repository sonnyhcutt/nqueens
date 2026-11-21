cmake_minimum_required(VERSION 3.14)
project(puzzles VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Game logic shared by the app and the tests.
add_library(puzzles_core
  src/board_io.cpp
  src/input.cpp
  src/knights_tour.cpp
  src/nqueens.cpp
  src/nqueens_solvers.cpp
)
target_include_directories(puzzles_core PUBLIC include)
if(MSVC)
  target_compile_options(puzzles_core PRIVATE /W4)
else()
  target_compile_options(puzzles_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(puzzles src/main.cpp)
target_link_libraries(puzzles PRIVATE puzzles_core)

add_executable(nqueens_bench bench/nqueens_bench.cpp)
target_link_libraries(nqueens_bench PRIVATE puzzles_core)

include(CTest)
if(BUILD_TESTING)
  add_executable(puzzles_tests tests/test_main.cpp)
  target_link_libraries(puzzles_tests PRIVATE puzzles_core)
  add_test(NAME puzzles_tests COMMAND puzzles_tests)
endif()
