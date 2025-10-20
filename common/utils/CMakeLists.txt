add_library(SLGL_COMMON_UTILS)
add_library(SLGL::COMMON::UTILS ALIAS SLGL_COMMON_UTILS)

set_target_properties(SLGL_COMMON_UTILS PROPERTIES LINKER_LANGUAGE CXX)

target_sources(SLGL_COMMON_UTILS
    PRIVATE
        src/Utils.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
        include/slgl/common/utils/ObserverPattern.hpp
)

target_link_libraries(SLGL_COMMON_UTILS
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_COMMON_UTILS
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)