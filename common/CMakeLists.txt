add_library(SLGL_COMMON INTERFACE)
add_library(SLGL::COMMON ALIAS SLGL_COMMON)

add_subdirectory(data)
add_subdirectory(math)
add_subdirectory(utils)

target_link_libraries(SLGL_COMMON
    INTERFACE
        SLGL::COMMON::DATA
        SLGL::COMMON::MATH
        SLGL::COMMON::UTILS
)