#!/usr/bin/env bash

# ------------------------------------------------------------------------------
# 1) Set the path to your vcpkg installation. Update if you installed vcpkg in
#    a different location (e.g., $HOME/src/vcpkg).
# ------------------------------------------------------------------------------
VCPKG_ROOT="$HOME/vcpkg"

# ------------------------------------------------------------------------------
# 2) Choose the triplet. Common Linux triplets:
#    - x64-linux
#    - x64-linux-musl
#    - x64-osx (for macOS)
# ------------------------------------------------------------------------------
VCPKG_TRIPLET="x64-linux"

# ------------------------------------------------------------------------------
# 3) Create a build directory (if it doesn't exist) and navigate into it.
# ------------------------------------------------------------------------------
[ ! -d "build" ] && mkdir build
cd build

# ------------------------------------------------------------------------------
# 4) Run CMake configuration, specifying:
#    - the vcpkg toolchain file
#    - the chosen triplet
# ------------------------------------------------------------------------------
cmake -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
      -DVCPKG_TARGET_TRIPLET="$VCPKG_TRIPLET" \
      ..

# ------------------------------------------------------------------------------
# 5) Build the project. For single-config generators (like Makefiles or Ninja),
#    this builds in the default build type. For multi-config (Xcode), specify.
# ------------------------------------------------------------------------------
cmake --build .

# For a Release build, you can do:
# cmake --build . --config Release

# Return to the original directory
cd ..
