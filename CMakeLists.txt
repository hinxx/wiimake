# minimum required for target_include_directories
cmake_minimum_required (VERSION 3.0.2)

# set compiler flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic -U__STRICT_ANSI__" CACHE STRING "g++ flags")

# link statically
set(BUILD_SHARED_LIBRARIES OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

# project name
project (WiiMake)

# project version
set (WiiMake_VERSION_MAJOR 1)
set (WiiMake_VERSION_MINOR 5)
set (WiiMake_VERSION_REVISION 0)

# create header for version number
configure_file (
    "src/MainProgram/Version.h.in"
    "${PROJECT_BINARY_DIR}/Version.h")

# add header to include path
include_directories ("${PROJECT_BINARY_DIR}")

# add directory with source files
add_subdirectory (src)

# set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
# set (CPACK_PACKAGE_VERSION_MAJOR "${WiiMake_VERSION_MAJOR}")
# set (CPACK_PACKAGE_VERSION_MINOR "${WiiMake_VERSION_MINOR}")
# include (CPack)


