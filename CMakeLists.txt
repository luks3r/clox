cmake_minimum_required(VERSION 3.25)
project(clox C)

set(CMAKE_C_STANDARD 23)

add_executable(clox main.c chunk.c memory.c debug.c value.c vm.c compiler.c scanner.c)
target_include_directories(clox PRIVATE ./)
