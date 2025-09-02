include(FetchContent)

add_library(SLGL_COMMON_MATH)
add_library(SLGL::COMMON::MATH ALIAS SLGL_COMMON_MATH)

set_target_properties(SLGL_COMMON_MATH PROPERTIES LINKER_LANGUAGE CXX)

target_sources(SLGL_COMMON_MATH
    PRIVATE
        src/Math.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/common/Math.hpp
)

set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 1.0.1
        GIT_SHALLOW ON
        GIT_PROGRESS ON
)
FetchContent_MakeAvailable(glm)

target_link_libraries(SLGL_COMMON_MATH
    PRIVATE
        slgl_warnings
        glm
)

target_include_directories(SLGL_COMMON_MATH
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
           ${glm_SOURCE_DIR}
)