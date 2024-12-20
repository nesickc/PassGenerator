#!/usr/bin/env bash

set -e

# Create the build directory if it doesn't exist
mkdir -p build

cd build

# Remove everything except the 'deps' directory if it exists
# If you don't have a 'deps' directory or don't need to preserve anything, you can simplify this.
find . -mindepth 1 -maxdepth 1 ! -name _deps -exec rm -rf {} +

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build .
