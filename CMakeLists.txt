cmake_minimum_required(VERSION 4.0)

# Project name and language
project(schmetterling LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

include_directories(include)

# Add the executable target
add_executable(schmetterling_exec
    src/main.cpp
    thirdparty/backward.cpp
    src/board/board.cpp
    src/board/fen.cpp
)

add_library(
    schmetterling
    src/main.cpp
    src/board/board.cpp
    src/board/fen.cpp
)

# Add test subdirectory
# add_subdirectory(test)

# Add test executable
add_executable(runUnitTests
    test/board/test_bitboard.cpp
    test/board/test_squares.cpp
    test/board/test_fen.cpp
    test/board/test_board.cpp
    test/moves/test_moves.cpp
)

# Link GoogleTest and your library
target_link_libraries(runUnitTests gtest_main schmetterling)

# Add the test
add_test(NAME UnitTests COMMAND runUnitTests)
