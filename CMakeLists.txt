cmake_minimum_required(VERSION 3.21)
project(PassGen LANGUAGES CXX)

# Optionally specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# 1) Tell CMake to use the vcpkg toolchain.
#    Usually you pass this as a command-line argument like:
#      cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg.cmake
#
#    But you can also hard-code it here if you prefer:
#
# set(CMAKE_TOOLCHAIN_FILE "/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake")

#
# 2) Find the libraries via CMake's find_package calls.
#    Because we are using vcpkg manifest mode, these libraries become available
#    as Config packages (e.g., "imguiConfig.cmake", "glfw3Config.cmake", etc.).
#

# Glad
find_package(glad CONFIG REQUIRED)

# libsodium
# vcpkg’s libsodium port provides the target: libsodium::libsodium
find_package(unofficial-sodium CONFIG REQUIRED)

# ImGui
# vcpkg’s imgui port provides the target: imgui::imgui
find_package(imgui CONFIG REQUIRED)

# GLFW
# vcpkg’s glfw3 port provides the target: glfw
find_package(glfw3 CONFIG REQUIRED)

# GLM
# vcpkg’s glm port typically provides the target: glm::glm
find_package(glm CONFIG REQUIRED)

#
# 3) Add your executable and link the libraries.
#

set(HEADERS
    src/UI.hpp
    src/Application.hpp
    src/PassGenerator.hpp
    src/PlatformManager.hpp
)

set(SOURCES
    src/main.cpp
    src/UI.cpp
    src/Application.cpp
    src/PassGenerator.cpp
    src/PlatformManager.cpp
)


add_executable(passGen ${HEADERS} ${SOURCES})

# Link all the libraries you need:
# - glad::glad
# - libsodium::libsodium
# - imgui::imgui
# - glfw
# - glm::glm

target_link_libraries(passGen
    PRIVATE
        glad::glad
        unofficial-sodium::sodium
        imgui::imgui
        glfw
        glm::glm
)

