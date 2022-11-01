cmake_minimum_required(VERSION 3.23)
project(quicksort)

set(CMAKE_CXX_STANDARD 14)

SET (CMAKE_CXX_COMPILER "D:/mingw64/bin/g++")
set(CMAKE_BUILD_TYPE "Debug")
set(COMPILE_FLAGS "-Wall -Wextra -Wpedantic -Wno-unused-parameter --std=c++17 -g")

add_executable(quicksort quicksort.cpp)