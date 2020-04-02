cmake_minimum_required(VERSION 3.10)
project(semaphore)

set(CMAKE_CXX_STANDARD 17)

add_executable(main main.cpp )
add_executable(semaphores sem_posix_test.cpp)
add_executable(readlinetest readlinetest.cpp)
target_link_libraries(semaphores rt)
target_link_libraries(semaphores pthread)

target_link_libraries(main rt)
target_link_libraries(main pthread)
