add_library(SLGL_RUNTIME_PLATFORM)
add_library(SLGL::RUNTIME::PLATFORM ALIAS SLGL_RUNTIME_PLATFORM)

target_sources(SLGL_RUNTIME_PLATFORM
    PRIVATE
        src/Platform.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/runtime/platform/Platform.hpp
        include/slgl/runtime/platform/IWindow.hpp
)

target_link_libraries(SLGL_RUNTIME_PLATFORM
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_RUNTIME_PLATFORM
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)