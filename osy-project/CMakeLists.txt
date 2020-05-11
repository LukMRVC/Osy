cmake_minimum_required(VERSION 3.10)
project(osy_project)

set(CMAKE_CXX_STANDARD 17)

add_executable(client client.cpp lib/Protocol.h lib/ProgramArgs.h lib/Logger.h lib/Utils.cpp)
add_executable(proc_srv proc_server.cpp lib/Protocol.h lib/ProgramArgs.h lib/Logger.h)
add_executable(thread_srv thread_server.cpp lib/Protocol.h lib/ProgramArgs.h lib/Logger.h)

target_link_libraries(client rt pthread)
target_link_libraries(proc_srv rt pthread)
target_link_libraries(thread_srv rt pthread)