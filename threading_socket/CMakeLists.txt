cmake_minimum_required(VERSION 3.10)
project(threading_socket)

set(CMAKE_CXX_STANDARD 17)

add_executable(threading_socket_server main.cpp FileDescriptorReader.h BufferedFileDescriptorReader.h)
add_executable(socket_client socket_cl.cpp)

target_link_libraries(threading_socket_server pthread)