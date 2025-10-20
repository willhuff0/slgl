cmake_minimum_required(VERSION 3.31)
project(slgl LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

# options
option(SLGL_BUILD_GENERATOR "Build offline project/editor tools library" ON)
option(SLGL_BUILD_RUNTIME "Build project runner executable" ON)

add_library(slgl_warnings INTERFACE)

# submodules
add_subdirectory(common)
if(SLGL_BUILD_GENERATOR)
    add_subdirectory(generator)
endif()
if(SLGL_BUILD_RUNTIME)
    add_subdirectory(runtime)
endif()