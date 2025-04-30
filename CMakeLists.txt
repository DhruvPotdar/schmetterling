cmake_minimum_required(VERSION 4.0)

# Project name and language
project(schmetterling LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)
# Add the executable target
add_executable(schmetterling
src/board/board.cpp
    # Add your other source files here, e.g.
    # src/board.cpp
    # src/engine.cpp
)

# No external dependencies, so no target_link_libraries needed
