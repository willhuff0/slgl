add_library(SLGL_RUNTIME_WSI)
add_library(SLGL::RUNTIME::WSI ALIAS SLGL_RUNTIME_WSI)

target_sources(SLGL_RUNTIME_WSI
    PRIVATE
        src/WSI.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/runtime/wsi/WSI.hpp
)

target_link_libraries(SLGL_RUNTIME_WSI
    PUBLIC
        SLGL::RUNTIME::PLATFORM
        SLGL::RUNTIME::GRAPHICS
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_RUNTIME_WSI
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

if (SLGL_PLATFORM_BACKEND_GLFW AND SLGL_GRAPHICS_BACKEND_VULKAN)
    add_subdirectory(backends/GLFW_Vulkan)
endif ()