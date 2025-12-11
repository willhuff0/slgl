add_library(SLGL_COMMON_RESOURCES)
add_library(SLGL::COMMON::RESOURCES ALIAS SLGL_COMMON_RESOURCES)

set_target_properties(SLGL_COMMON_RESOURCES PROPERTIES LINKER_LANGUAGE CXX)

target_sources(SLGL_COMMON_RESOURCES
    PRIVATE
        src/Resources.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/common/resources/Resource.hpp
            include/slgl/common/resources/Model.hpp
)

target_link_libraries(SLGL_COMMON_RESOURCES
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_COMMON_RESOURCES
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)