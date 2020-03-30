cmake_minimum_required(VERSION 3.15)
project(socket)

set(CMAKE_CXX_STANDARD 17)

add_executable(socket_client socket_cl.cpp)
add_executable(socket_server socket_srv.cpp)
add_executable(readline readline.cpp)