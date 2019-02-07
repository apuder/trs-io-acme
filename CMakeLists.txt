# cmake_minimum_required(VERSION <specify CMake version here>)
project(TrsIO)

set(CMAKE_CXX_STANDARD 14)

add_executable(TrsIO main.cpp trsio.h trsio.cpp acme.cpp)