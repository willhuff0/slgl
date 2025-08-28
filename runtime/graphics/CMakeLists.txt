add_library(SLGL_RUNTIME_GRAPHICS)
add_library(SLGL::RUNTIME::GRAPHICS ALIAS SLGL_RUNTIME_GRAPHICS)

target_sources(SLGL_RUNTIME_GRAPHICS
    PRIVATE
        src/GFX.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/runtime/graphics/GFX.hpp
            include/slgl/runtime/graphics/ISurface.hpp
            include/slgl/runtime/graphics/Flags.h
            include/slgl/runtime/graphics/Queue.h
            include/slgl/runtime/graphics/RenderEncoder.h
            include/slgl/runtime/graphics/Buffer.h
            include/slgl/runtime/graphics/Texture.h
            include/slgl/runtime/graphics/Sampler.h
            include/slgl/runtime/graphics/BindSet.h
            include/slgl/runtime/graphics/ShaderModule.h
            include/slgl/runtime/graphics/ShaderPipeline.h
            include/slgl/runtime/graphics/RenderPass.hpp
)

target_link_libraries(SLGL_RUNTIME_GRAPHICS
    PUBLIC
        SLGL::COMMON
        SLGL::RUNTIME::PLATFORM
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_RUNTIME_GRAPHICS
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)

# backend options
option(SLGL_GFX_BACKEND_VULKAN "Enable Vulkan backend" OFF)
option(SLGL_GFX_BACKEND_WEBGPU "Enable WebGPU backend" ON)

target_compile_definitions(SLGL_RUNTIME_GRAPHICS
    PUBLIC
        $<$<BOOL:${SLGL_GFX_BACKEND_VULKAN}>:SLGL_GFX_BACKEND_VULKAN>
        $<$<BOOL:${SLGL_GFX_BACKEND_WEBGPU}>:SLGL_GFX_BACKEND_WEBGPU>
)

if (SLGL_GFX_BACKEND_VULKAN)
    add_subdirectory(backends/Vulkan)
endif()
if (SLGL_GFX_BACKEND_WEBGPU)
    add_subdirectory(backends/WebGPU)
endif()