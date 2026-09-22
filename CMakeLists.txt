cmake_minimum_required(VERSION 3.10)

project(CNav C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")

  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Standard Build-Typ is Release" FORCE)
endif()

set(SOURCES
    src/main.c
    src/add.c
    src/search.c
    src/init.c
    src/clean.c
    src/platform.c 
    src/sqlite3.c
    src/list.c
)

add_executable(n ${SOURCES})

target_include_directories(n PRIVATE inc)

if (MSVC)
    target_compile_options(n PRIVATE /W4)
else()
    target_compile_options(n PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_definitions(n PRIVATE _XOPEN_SOURCE=700) 
endif()

if (MSVC)
    set_source_files_properties(src/sqlite3.c PROPERTIES COMPILE_FLAGS "/W0")
else()
    set_source_files_properties(src/sqlite3.c PROPERTIES COMPILE_FLAGS "-w")
endif()

find_package(Threads REQUIRED)

if (UNIX)
    target_link_libraries(n PRIVATE Threads::Threads dl)
else()
    target_link_libraries(n PRIVATE Threads::Threads)
endif()
