cmake_minimum_required(VERSION 3.10)
project(readline CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(readline main.cpp FileDescriptorReader.h BufferedFileDescriptorReader.h)