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
            include/slgl/runtime/platform/Backend.hpp
            include/slgl/runtime/platform/Window.hpp
)

target_link_libraries(SLGL_RUNTIME_PLATFORM
    PUBLIC
        SLGL::COMMON
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_RUNTIME_PLATFORM
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)

# backend options
option(SLGL_PLATFORM_BACKEND_SDL3 "Enable SDL3 platform backend" ON)
option(SLGL_PLATFORM_BACKEND_GLFW "Enable GLFW platform backend" OFF)

target_compile_definitions(SLGL_RUNTIME_PLATFORM
    PUBLIC
        $<$<BOOL:${SLGL_PLATFORM_BACKEND_SDL3}>:SLGL_PLATFORM_BACKEND_SDL3>
        $<$<BOOL:${SLGL_PLATFORM_BACKEND_GLFW}>:SLGL_PLATFORM_BACKEND_GLFW>
)

if (SLGL_PLATFORM_BACKEND_SDL3)
    add_subdirectory(backends/SDL3)
endif()
if (SLGL_PLATFORM_BACKEND_GLFW)
    add_subdirectory(backends/GLFW)
endif()
