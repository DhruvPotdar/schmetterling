#!/usr/bin/env bash

# Exit on error
set -e

BUILD_DIR="build"

# Check for clean option
if [[ "$1" == "clean" ]]; then
  echo "[INFO] Removing old build..."
  rm -rf "$BUILD_DIR"
  echo "[INFO] Clean successful!"
  exit 0
fi

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
  echo "[INFO] Creating build directory..."
  mkdir "$BUILD_DIR"
fi

# Enter build directory
echo "[INFO] Entering build directory..."
cd "$BUILD_DIR"

# Run CMake and make
echo "[INFO] Running CMake..."
cmake ..
echo "[INFO] Compiling with make..."
cmake --build . -j"$(nproc)"

# Indicator before execution
echo "==================================="
echo " Build successful! Running schmetterling..."
echo "==================================="

# Run the program
./schmetterling_exec
