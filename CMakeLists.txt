cmake_minimum_required(VERSION 3.10)
project(CNav C)

# ==============================================================================
# 1. Global Settings (Standard & Build Type)
# ==============================================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Set Release as the default build type if nothing else is specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type is Release" FORCE)
endif()

# Option for AddressSanitizer (OFF by default)
option(ENABLE_ASAN "Enable AddressSanitizer for memory debugging" OFF)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")

# ==============================================================================
# 2. Define Target (Source files and Executable)
# ==============================================================================
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

# Specify include directories
target_include_directories(n PRIVATE inc)

# ==============================================================================
# 3. Compiler Warnings and Platform Flags
# ==============================================================================
if(MSVC)
  # Windows/MSVC specifics
  target_compile_options(n PRIVATE /W4)
  target_compile_definitions(n PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
  # Linux/macOS specifics (GCC/Clang)
  target_compile_options(n PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_definitions(n PRIVATE _XOPEN_SOURCE=700)

  # Enable AddressSanitizer only if requested by the user
  if(ENABLE_ASAN)
    target_compile_options(n PRIVATE -g -fsanitize=address)
    target_link_options(n PRIVATE -fsanitize=address)
  endif()
endif()

# ==============================================================================
# 4. Exceptions & External Libraries
# ==============================================================================
# Completely disable warnings for the SQLite amalgamation file
if(MSVC)
  set_source_files_properties(src/sqlite3.c PROPERTIES COMPILE_FLAGS "/W0")
else()
  set_source_files_properties(src/sqlite3.c PROPERTIES COMPILE_FLAGS "-w")
endif()

# Link Threads and dynamic linking (dl)
find_package(Threads REQUIRED)
if(UNIX)
  target_link_libraries(n PRIVATE Threads::Threads dl)
else()
  target_link_libraries(n PRIVATE Threads::Threads)
endif()


# ==============================================================================
# 5. Testing
# ==============================================================================
enable_testing()

add_executable(test_search tests/test_search.c src/platform.c src/sqlite3.c)

target_include_directories(test_search PRIVATE inc)

if(UNIX)
  target_link_libraries(test_search PRIVATE Threads::Threads dl)
else()
  target_link_libraries(test_search PRIVATE Threads::Threads)
endif()

add_test(NAME SearchTest COMMAND test_search)

# ==========================================
# Database Integration Tests
# ==========================================
add_executable(test_database
    tests/test_database.c
    src/search.c
    src/add.c
    src/init.c
    src/clean.c
    src/sqlite3.c
)

target_include_directories(test_database PRIVATE inc)

if(UNIX)
  target_link_libraries(test_database PRIVATE Threads::Threads dl)
else()
  target_link_libraries(test_database PRIVATE Threads::Threads)
endif()

add_test(NAME DatabaseTest COMMAND test_database)
