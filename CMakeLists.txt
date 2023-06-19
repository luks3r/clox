cmake_minimum_required(VERSION 3.25)
project(clox CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

add_compile_options(
        -Wall
        -Wextra
        -pedantic
        )

add_executable(clox
        src/main.cc
        src/chunk.cc include/chunk.hh
        src/memory.cc include/memory.hh
        src/debug.cc include/debug.hh
        src/value.cc include/value.hh
        src/vm.cc include/vm.hh
        src/compiler.cc include/compiler.hh
        src/scanner.cc include/scanner.hh
        src/object.cc include/object.hh
        )
target_include_directories(clox PRIVATE include)
