cmake_minimum_required(VERSION 3.27)

# Project name and language
project(schmetterling LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Release)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG main  # Use a specific release tag (e.g., v1.14.0) for stability
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
    src/moves/generation/move_generation.cpp
    src/evaluation/evaluation.cpp
)

add_library(
    schmetterling
    src/main.cpp
    src/board/board.cpp
    src/board/fen.cpp
    src/moves/generation/move_generation.cpp
    src/evaluation/evaluation.cpp
)

# Add perft executable
add_executable(perft
    src/perft.cpp
    src/board/board.cpp
    src/board/fen.cpp
    src/moves/generation/move_generation.cpp
    src/evaluation/evaluation.cpp
)

# Add test executable
add_executable(runUnitTests
    test/board/test_squares.cpp
    test/moves/test_moves.cpp
    test/board/test_bitboard.cpp
    test/board/test_fen.cpp
    test/board/test_board.cpp
    test/moves/test_move_generation.cpp
    # test/evaluation/test_evaluation.cpp
)
# Link GoogleTest and your library
target_link_libraries(runUnitTests gtest_main schmetterling)

# Add the test
add_test(NAME UnitTests COMMAND runUnitTests)
