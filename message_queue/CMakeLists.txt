cmake_minimum_required(VERSION 3.10)
project(message_queue)

set(CMAKE_CXX_STANDARD 17)

add_executable(message_queue main.cpp)
add_executable(msq_posix msg_posix_test.cpp)
add_executable(msq_sysv msg_sysv_test.cpp)
add_executable(shm shm_posix_test.cpp)


target_link_libraries(message_queue rt)
target_link_libraries(msq_posix rt)
target_link_libraries(msq_sysv rt)
target_link_libraries(shm rt)