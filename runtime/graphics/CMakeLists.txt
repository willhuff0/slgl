add_library(SLGL_RUNTIME_GRAPHICS)
add_library(SLGL::RUNTIME::GRAPHICS ALIAS SLGL_RUNTIME_GRAPHICS)

target_sources(SLGL_RUNTIME_GRAPHICS
    PRIVATE
        src/Graphics.cpp
        src/BindSetBuilder.cpp
        src/BufferBuilder.cpp
        src/SamplerBuilder.cpp
        src/ShaderModuleBuilder.cpp
        src/ShaderPipelineBuilder.cpp
        src/TextureBuilder.cpp
        src/MipmapGenerator.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/runtime/graphics/Graphics.hpp
            include/slgl/runtime/graphics/Backend.hpp
            include/slgl/runtime/graphics/Surface.hpp
            include/slgl/runtime/graphics/Flags.hpp
            include/slgl/runtime/graphics/Queue.hpp
            include/slgl/runtime/graphics/Buffer.hpp
            include/slgl/runtime/graphics/Texture.hpp
            include/slgl/runtime/graphics/Sampler.hpp
            include/slgl/runtime/graphics/BindSet.hpp
            include/slgl/runtime/graphics/ShaderModule.hpp
            include/slgl/runtime/graphics/ShaderPipeline.hpp
            include/slgl/runtime/graphics/CommandBuffer.hpp
            include/slgl/runtime/graphics/CommandEncoder.hpp
            include/slgl/runtime/graphics/RenderEncoder.hpp
            include/slgl/runtime/graphics/MipmapGenerator.hpp
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
option(SLGL_GRAPHICS_BACKEND_VULKAN "Enable Vulkan graphics backend" OFF)
option(SLGL_GRAPHICS_BACKEND_WEBGPU "Enable WebGPU graphics backend" ON)

target_compile_definitions(SLGL_RUNTIME_GRAPHICS
    PUBLIC
        $<$<BOOL:${SLGL_GRAPHICS_BACKEND_VULKAN}>:SLGL_GRAPHICS_BACKEND_VULKAN>
        $<$<BOOL:${SLGL_GRAPHICS_BACKEND_WEBGPU}>:SLGL_GRAPHICS_BACKEND_WEBGPU>
)

if (SLGL_GRAPHICS_BACKEND_VULKAN)
    add_subdirectory(backends/Vulkan)
endif()
if (SLGL_GRAPHICS_BACKEND_WEBGPU)
    add_subdirectory(backends/WebGPU)
endif()