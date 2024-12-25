@echo off

REM ----------------------------------------------------------------------------
REM Check if the VCPKG_ROOT environment variable is set. If not, you can either
REM error out, or set a default path. Here we default to "C:\dev\vcpkg".
REM ----------------------------------------------------------------------------
if not defined VCPKG_ROOT (
    echo "VCPKG_ROOT is not defined. Defaulting to C:\dev\vcpkg"
    set "VCPKG_ROOT=C:\dev\vcpkg"
)

echo Using VCPKG_ROOT=%VCPKG_ROOT%

REM ----------------------------------------------------------------------------
REM Choose the vcpkg triplet. e.g., x64-windows or x64-windows-static
REM ----------------------------------------------------------------------------
set "VCPKG_TRIPLET=x64-windows"

REM ----------------------------------------------------------------------------
REM Create a build directory (if it doesn't exist) and enter it.
REM ----------------------------------------------------------------------------
if not exist build (
    mkdir build
)
cd build

REM ----------------------------------------------------------------------------
REM Run CMake:
REM  1) point to vcpkg toolchain using the environment variable
REM  2) set the triplet
REM ----------------------------------------------------------------------------
cmake -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" ^
      -DVCPKG_TARGET_TRIPLET=%VCPKG_TRIPLET% ^
      ..

REM ----------------------------------------------------------------------------
REM Build the project (default config). 
REM For Visual Studio generators, you can specify --config Release if you want.
REM ----------------------------------------------------------------------------
cmake --build .
REM cmake --build . --config Release

REM Return to the original directory
cd ..
