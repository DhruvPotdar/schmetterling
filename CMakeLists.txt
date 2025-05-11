cmake_minimum_required(VERSION 4.0)

# Project name and language
project(schmetterling LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_subdirectory(tests)
include_directories(include)
# Add the executable target
add_executable(schmetterling_exec
    src/main.cpp
    src/board/board.cpp
    src/helpers/fen.cpp
    # Add your other source files here, e.g.
    # src/board.cpp
    # src/engine.cpp
)

add_library(
    schmetterling src/main.cpp
    src/board/board.cpp
    src/helpers/fen.cpp
)

