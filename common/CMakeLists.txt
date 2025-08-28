add_library(SLGL_COMMON INTERFACE)
add_library(SLGL::COMMON ALIAS SLGL_COMMON)

add_subdirectory(data)

target_link_libraries(SLGL_COMMON
    INTERFACE
        SLGL::COMMON::DATA
)