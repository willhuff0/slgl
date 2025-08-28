add_library(SLGL_RUNTIME_RESOURCE_READER)
add_library(SLGL::RUNTIME::RESOURCE_READER ALIAS SLGL_RUNTIME_RESOURCE_READER)

target_sources(SLGL_RUNTIME_RESOURCE_READER
    PRIVATE
        src/ResourceReader.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/runtime/resource_reader/ResourceReader.hpp
)

target_link_libraries(SLGL_RUNTIME_RESOURCE_READER
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_RUNTIME_RESOURCE_READER
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)