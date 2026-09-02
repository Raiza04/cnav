cmake_minimum_required(VERSION 3.10)

project(CNav C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(SOURCES
    src/main.c
    src/add.c
    src/search.c
    src/init.c
    src/clean.c
)

add_executable(n ${SOURCES})

target_include_directories(n PRIVATE inc)

target_compile_options(n PRIVATE -Wall -Wextra -Wpedantic)