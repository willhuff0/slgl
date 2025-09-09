include(FetchContent)

set(SLGL_WEBGPU_BACKEND "DAWN" CACHE STRING "Implementation of WebGPU. Possible values are DAWN and WGPU")
set_property(CACHE SLGL_WEBGPU_BACKEND PROPERTY STRINGS "DAWN" "WGPU")
if (SLGL_WEBGPU_BACKEND STREQUAL "DAWN")
    set(WEBGPU_BACKEND "DAWN" CACHE STRING "" FORCE)
elseif (SLGL_WEBGPU_BACKEND STREQUAL "WGPU")
    set(WEBGPU_BACKEND "WGPU" CACHE STRING "" FORCE)
else ()
    set(WEBGPU_BACKEND "DAWN" CACHE STRING "" FORCE)
endif ()

FetchContent_Declare(webgpu
    URL      https://github.com/eliemichel/WebGPU-distribution/releases/download/v0.3.0-gamma/WebGPU-distribution-v0.3.0-gamma.zip
    URL_HASH SHA256=c26287b1889d0534459860aa26f97578cb971245469c9202dfe58fc3b6702dd6
)
FetchContent_MakeAvailable(webgpu)
target_link_libraries(SLGL_RUNTIME_GRAPHICS PRIVATE webgpu)
add_custom_target(copy_webgpu_binaries ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${WEBGPU_RUNTIME_LIB}
                $<TARGET_FILE_DIR:SLGL_RUNTIME_EXECUTABLE>
        COMMENT "Copying WebGPU binaries to executable directory"
)
add_dependencies(SLGL_RUNTIME_EXECUTABLE copy_webgpu_binaries)

target_sources(SLGL_RUNTIME_GRAPHICS
    PRIVATE
        src/webgpu_cpp_impl.cpp
        src/WebGPUDebug.cpp
        src/WebGPUContext.cpp
        src/WebGPULimits.cpp
        src/WebGPUCreators.cpp
        src/WebGPUConverters.cpp
        src/WebGPUBackend.cpp
        src/WebGPUSurface.cpp
        src/WebGPURenderEncoder.cpp
        src/WebGPUBuffer.cpp
        src/WebGPUTexture.cpp
        src/WebGPUTextureView.cpp
        src/WebGPUSampler.cpp
        src/WebGPUBindSet.cpp
        src/WebGPUBindSetLayout.cpp
        src/WebGPUShaderModule.cpp
        src/WebGPUShaderPipelineLayout.cpp
        src/WebGPUShaderRenderPipeline.cpp
        src/WebGPUCommandBuffer.cpp
        src/WebGPUCommandEncoder.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/runtime/graphics/backends/WebGPU/WebGPUDebug.hpp
            include/slgl/runtime/graphics/backends/WebGPU/WebGPUContext.hpp
            include/slgl/runtime/graphics/backends/WebGPU/WebGPULimits.hpp
            include/slgl/runtime/graphics/backends/WebGPU/WebGPUConverters.hpp
            include/slgl/runtime/graphics/backends/WebGPU/WebGPUBackend.hpp
            include/slgl/runtime/graphics/backends/WebGPU/WebGPUSurface.hpp
            include/slgl/runtime/graphics/backends/WebGPU/WebGPURenderEncoder.hpp
            include/slgl/runtime/graphics/backends/WebGPU/WebGPUBuffer.hpp
            include/slgl/runtime/graphics/backends/WebGPU/WebGPUTexture.hpp
            include/slgl/runtime/graphics/backends/WebGPU/WebGPUSampler.hpp
            include/slgl/runtime/graphics/backends/WebGPU/WebGPUBindSet.hpp
            include/slgl/runtime/graphics/backends/WebGPU/WebGPUShaderModule.hpp
            include/slgl/runtime/graphics/backends/WebGPU/WebGPUShaderPipelineLayout.hpp
            include/slgl/runtime/graphics/backends/WebGPU/WebGPUShaderRenderPipeline.hpp
            include/slgl/runtime/graphics/backends/WebGPU/WebGPUCommandBuffer.hpp
            include/slgl/runtime/graphics/backends/WebGPU/WebGPUCommandEncoder.hpp
)

target_include_directories(SLGL_RUNTIME_GRAPHICS
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
    PRIVATE
        include/slgl/runtime/graphics
)