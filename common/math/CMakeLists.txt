add_library(SLGL_COMMON_MATH)
add_library(SLGL::COMMON::MATH ALIAS SLGL_COMMON_MATH)

target_sources(SLGL_COMMON_MATH
    PRIVATE
        src/Math.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/common/Math.hpp
)

find_package(glm 1.0.1 CONFIG REQUIRED)
target_compile_definitions(SLGL_COMMON_MATH
    PUBLIC
        GLM_ENABLE_EXPERIMENTAL
)

target_link_libraries(SLGL_COMMON_MATH
    PUBLIC
        glm::glm
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_COMMON_MATH
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)