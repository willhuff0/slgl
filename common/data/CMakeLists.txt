add_library(SLGL_COMMON_DATA)
add_library(SLGL::COMMON::DATA ALIAS SLGL_COMMON_DATA)

set_target_properties(SLGL_COMMON_DATA PROPERTIES LINKER_LANGUAGE CXX)

target_sources(SLGL_COMMON_DATA
    PRIVATE
        src/Image.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/common/data/Data.hpp
            include/slgl/common/data/Image.hpp
            include/slgl/common/data/Shader.hpp
)

target_link_libraries(SLGL_COMMON_DATA
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_COMMON_DATA
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)