add_library(SLGL_GENERATOR)
add_library(SLGL::GENERATOR ALIAS SLGL_GENERATOR)

set_target_properties(SLGL_GENERATOR PROPERTIES LINKER_LANGUAGE CXX)

target_sources(SLGL_GENERATOR
    PRIVATE
        src/Generator.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/generator/Generator.hpp
)

target_link_libraries(SLGL_GENERATOR
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_GENERATOR
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)