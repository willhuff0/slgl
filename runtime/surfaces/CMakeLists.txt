add_library(SLGL_RUNTIME_SURFACES)
add_library(SLGL::RUNTIME::SURFACES ALIAS SLGL_RUNTIME_SURFACES)

target_sources(SLGL_RUNTIME_SURFACES
    PRIVATE
        src/Surfaces.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/runtime/surfaces/Surfaces.hpp
)

target_link_libraries(SLGL_RUNTIME_SURFACES
    PUBLIC
        SLGL::RUNTIME::PLATFORM
        SLGL::RUNTIME::GRAPHICS
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_RUNTIME_SURFACES
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)

# backend pairs
if (SLGL_PLATFORM_BACKEND_SDL3 AND SLGL_GRAPHICS_BACKEND_WEBGPU)
    add_subdirectory(backends/SDL3_WebGPU)
endif ()

if (SLGL_PLATFORM_BACKEND_GLFW AND SLGL_GRAPHICS_BACKEND_WEBGPU)
    add_subdirectory(backends/GLFW_WebGPU)
endif ()