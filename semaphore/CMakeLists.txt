cmake_minimum_required(VERSION 3.15)
project(semaphore)

set(CMAKE_CXX_STANDARD 17)


add_executable(main main.cpp)
add_executable(semaphores sem_posix_test.cpp)
target_link_libraries(semaphores rt)
target_link_libraries(semaphores pthread)
