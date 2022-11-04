cmake_minimum_required(VERSION 3.23)
project(quicksort)

set(CMAKE_CXX_STANDARD 14)

SET (CMAKE_CXX_COMPILER "C:/Program Files/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/g++.exe")
set(CMAKE_BUILD_TYPE "Debug")
set(COMPILE_FLAGS "-Wall -Wextra -Wpedantic -Wno-unused-parameter --std=c++17 -g")

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

add_executable(quicksort quicksort.cpp)