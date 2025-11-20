cmake_minimum_required(VERSION 3.10)
project(pphc VERSION 0.1.0 LANGUAGES C)

# Set C standard (relaxed for GCC/Clang compatibility)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS OFF)

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_STATIC_LIBS "Build static libraries" ON)
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_CLI "Build command-line tool" ON)
option(BUILD_WIN32_GUI "Build Win32 GUI application (Windows only)" OFF)

# Android configuration (must be before other includes)
if(ANDROID)
    include(cmake/AndroidConfig.cmake)
endif()

# Emscripten configuration (must be before other includes)
if(EMSCRIPTEN)
    include(cmake/EmscriptenConfig.cmake)
endif()

# Include CMake modules
include(cmake/CompilerDetection.cmake)
if(NOT ANDROID)
    include(cmake/PlatformConfig.cmake)
endif()
if(APPLE AND NOT EMSCRIPTEN AND NOT ANDROID)
    include(cmake/AppleFramework.cmake)
endif()

# Compiler detection and configuration
detect_compiler()
if(NOT EMSCRIPTEN AND NOT ANDROID)
    configure_platform()
endif()

# Add subdirectories
add_subdirectory(libpph)

if(BUILD_CLI)
    add_subdirectory(cli)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_WIN32_GUI AND WIN32)
    add_subdirectory(win32)
endif()

# Installation configuration
include(GNUInstallDirs)

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "pphc")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Indonesian Tax Calculator Library")
set(CPACK_PACKAGE_VENDOR "OpenPajak Contributors")
include(CPack)
