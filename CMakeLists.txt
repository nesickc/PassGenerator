cmake_minimum_required(VERSION 3.14)
project(PassGen LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)
include(ExternalProject)

# Set a directory where external projects are built and installed
set(EXTERNAL_INSTALL_DIR "${CMAKE_BINARY_DIR}/_deps")

# -----------------------
# Fetch glad
# -----------------------
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        658f48e72aee3c6582e80b05ac0f8787a64fe6bb #v2.0.6
    GIT_PROGRESS   TRUE
    SOURCE_SUBDIR  cmake
)

FetchContent_MakeAvailable(glad)

glad_add_library(glad_gl_core_43 STATIC REPRODUCIBLE LOADER API gl:core=4.3)

# -----------------------
# Fetch libsodium
# -----------------------
FetchContent_Declare(
  libsodium
  # Use URL
  # URL https://download.libsodium.org/libsodium/releases/libsodium-1.0.20.tar.gz
  # Or use the Git repo:
  GIT_REPOSITORY https://github.com/jedisct1/libsodium.git
  GIT_TAG        stable
)

FetchContent_GetProperties(libsodium)
if(NOT libsodium_POPULATED)
  FetchContent_Populate(libsodium)

  if(WIN32)
    # On Windows, use the Visual Studio project directly
    # Adjust the Platform and Configuration if needed (x64, Win32, Debug, Release)
    set(LIBSODIUM_MSVC_DIR "${libsodium_SOURCE_DIR}/builds/msvc/vs2022/libsodium")

    ExternalProject_Add(libsodium-project
      SOURCE_DIR ${LIBSODIUM_MSVC_DIR}
      CONFIGURE_COMMAND ""
      BUILD_COMMAND msbuild libsodium.vcxproj /p:Configuration=ReleaseLIB /p:Platform=x64
      INSTALL_COMMAND ""
      BUILD_IN_SOURCE 1
    )

    # For linking, you need to know where the built .lib will be placed.
    # Should be in ${libsodium_SOURCE_DIR}/bin/x64/Release/ (adjust if needed).
    # After build, you could copy or reference directly:
    set(LIBSODIUM_LIB "${libsodium_SOURCE_DIR}/bin/x64/Release/v143/static/libsodium.lib")
    set(LIBSODIUM_INCLUDE_DIR "${libsodium_SOURCE_DIR}/src/libsodium/include")

    add_library(libsodium STATIC IMPORTED)
    add_dependencies(libsodium libsodium-project)
    set_target_properties(libsodium PROPERTIES
      IMPORTED_LOCATION "${LIBSODIUM_LIB}"
      INTERFACE_INCLUDE_DIRECTORIES "${LIBSODIUM_INCLUDE_DIR}"
    )

  else()
    # On Unix-like systems, run configure and make
    ExternalProject_Add(libsodium-project
      SOURCE_DIR ${libsodium_SOURCE_DIR}
      PREFIX ${EXTERNAL_INSTALL_DIR}/libsodium
      CONFIGURE_COMMAND ./configure --prefix=${EXTERNAL_INSTALL_DIR}/libsodium --disable-shared
      BUILD_COMMAND make
      INSTALL_COMMAND make install
      BUILD_IN_SOURCE 1
    )

    add_library(libsodium STATIC IMPORTED)
    add_dependencies(libsodium libsodium-project)
    set_target_properties(libsodium PROPERTIES
      IMPORTED_LOCATION "${EXTERNAL_INSTALL_DIR}/libsodium/lib/libsodium.a"
      INTERFACE_INCLUDE_DIRECTORIES "${EXTERNAL_INSTALL_DIR}/libsodium/include"
    )
  endif()
endif()

# -----------------------
# Fetch ImGui + GLFW
# -----------------------
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(imgui)

set(IMGUI_SRC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
)

set(IMGUI_HDR
    ${imgui_SOURCE_DIR}/imgui.h
    ${imgui_SOURCE_DIR}/imconfig.h
    ${imgui_SOURCE_DIR}/imgui_internal.h
    ${imgui_SOURCE_DIR}/imstb_rectpack.h
    ${imgui_SOURCE_DIR}/imstb_textedit.h
    ${imgui_SOURCE_DIR}/imstb_truetype.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
)

add_library(imgui STATIC ${IMGUI_SRC} ${IMGUI_HDR})
target_include_directories(imgui PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(glfw)

target_link_libraries(imgui PUBLIC glfw)

if(WIN32)
    target_link_libraries(imgui PUBLIC opengl32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(imgui PUBLIC OpenGL::GL dl)
endif()

# -----------------------
# Create our executable
# -----------------------
add_executable(PassGen
  src/main.cpp
  src/UI.cpp
  src/UI.hpp
  src/PassGenerator.cpp
  src/PassGenerator.hpp
  src/Application.cpp
  src/Application.hpp
)

target_compile_definitions(PassGen PRIVATE SODIUM_STATIC)

# Ensure libsodium is built first
add_dependencies(PassGen libsodium-project)
target_link_libraries(PassGen PRIVATE libsodium imgui glad_gl_core_43)
target_include_directories(PassGen PRIVATE ${PROJECT_SOURCE_DIR}/src)
