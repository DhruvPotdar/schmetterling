# Justfile for the Schmetterling Chess Engine
# Manages building, testing, documentation, and running executables
BUILD_DIR := "build"

# Default build type (Release or Debug)
BUILD_TYPE := "Release"

# Default depth for perft
DEFAULT_PERFT_DEPTH := "5"

# Default FEN for perft
DEFAULT_FEN := "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

# Default task: list available tasks
default:
    just --list

# Clean the build directory
clean:
    @echo "[INFO] Removing build directory..."
    rm -rf {{BUILD_DIR}}
    @echo "[INFO] Clean successful!"

# Build the project (Release or Debug)
build type=BUILD_TYPE:
    @echo "[INFO] Creating build directory if needed..."
    mkdir -p {{BUILD_DIR}}
    @echo "[INFO] Entering build directory..."
    cd {{BUILD_DIR}} && \
    echo "[INFO] Running CMake with build type {{type}}..." && \
    cmake -Wall -DCMAKE_BUILD_TYPE={{type}} .. && \
    echo "[INFO] Compiling with make..." && \
    cmake --build . -j$(nproc)

# Run unit tests with colored output
test: build
    @echo "[INFO] Running unit tests..."
    export GTEST_COLOR=1 && cd {{BUILD_DIR}} && ctest --output-on-failure

# Generate Doxygen documentation
docs:
    @echo "[INFO] Generating Doxygen documentation..."
    doxygen Doxyfile
    @echo "[INFO] Documentation generated in docs/html/"

# Run the main engine executable
run: build
    @echo "[INFO] Running schmetterling engine..."
    {{BUILD_DIR}}/schmetterling_exec

# Run perft with specified depth and optional FEN
perft depth=DEFAULT_PERFT_DEPTH fen=DEFAULT_FEN: build
    @echo "[INFO] Running perft with depth {{depth}} and FEN {{fen}}..."
    {{BUILD_DIR}}/perft {{depth}} "{{fen}}"


# Build and run all tasks (build, test, docs, perft with default depth)
all: build test docs (perft DEFAULT_PERFT_DEPTH)
    @echo "[INFO] All tasks completed!"
