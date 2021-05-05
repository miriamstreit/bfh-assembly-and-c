cmake_minimum_required(VERSION 3.17)
project(io_and_control C)

set(CMAKE_C_STANDARD 99)

add_executable(io_and_control-m main.c)
add_executable(io_and_control-1 1.c)
add_executable(io_and_control-2 2.c)
add_executable(io_and_control-3 3.c)