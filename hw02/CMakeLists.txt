cmake_minimum_required(VERSION 3.15)
project(main)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(main sandbox.cpp main.cpp utils.h)