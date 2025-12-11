add_library(SLGL_COMMON_UTILS)
add_library(SLGL::COMMON::UTILS ALIAS SLGL_COMMON_UTILS)

set_target_properties(SLGL_COMMON_UTILS PROPERTIES LINKER_LANGUAGE CXX)

target_sources(SLGL_COMMON_UTILS
    PRIVATE
        src/Utils.cpp
        src/InputStream.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/common/utils/ObserverPattern.hpp
        include/slgl/common/utils/InputStream.hpp
            include/slgl/common/utils/Serialization.hpp
)

if (APPLE)
    include(FetchContent)
    FetchContent_Declare(stduuid
        URL      https://github.com/mariusbancila/stduuid/archive/refs/tags/v1.2.3.zip
        URL_HASH SHA256=0f867768ce55f2d8fa361be82f87f0ea5e51438bc47ca30cd92c9fd8b014e84e
    )
    FetchContent_MakeAvailable(stduuid)
else ()
    find_package(stduuid CONFIG REQUIRED)
endif ()

target_link_libraries(SLGL_COMMON_UTILS
    PUBLIC
        stduuid
        SLGL::COMMON::MATH
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_COMMON_UTILS
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)