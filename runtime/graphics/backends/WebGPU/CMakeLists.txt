include(FetchContent)

set(SLGL_WEBGPU_BACKEND "DAWN" CACHE STRING "Implementation of WebGPU. Possible values are DAWN and WGPU")
set_property(CACHE SLGL_WEBGPU_BACKEND PROPERTY STRINGS "DAWN" "WGPU")
if(SLGL_WEBGPU_BACKEND STREQUAL "DAWN")
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

target_include_directories(SLGL_RUNTIME_GRAPHICS PRIVATE headers)

target_sources(SLGL_RUNTIME_GRAPHICS
    PRIVATE
        src/webgpu_cpp_impl.cpp
        src/WebGPUDebug.cpp
        src/WebGPUContext.cpp
        src/WebGPULimits.cpp
        src/WebGPUFactories.cpp
        src/WebGPUConverters.cpp
        src/GFX/WebGPUGFX.cpp
        src/GFX/WebGPUSurface.cpp
        src/GFX/WebGPURenderEncoder.cpp
        src/GFX/WebGPUBuffer.cpp
        src/GFX/WebGPUTexture.cpp
        src/GFX/WebGPUTextureView.cpp
        src/GFX/WebGPUSampler.cpp
        src/GFX/WebGPUBindSet.cpp
        src/GFX/WebGPUBindSetLayout.cpp
        src/GFX/WebGPUShaderModule.cpp
        src/GFX/WebGPUShaderPipelineLayout.cpp
        src/GFX/WebGPUShaderRenderPipeline.cpp
        src/GFX/WebGPUCommandBuffer.cpp
        src/GFX/WebGPUCommandEncoder.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS headers
        FILES
            headers/WebGPUDebug.hpp
            headers/WebGPUContext.hpp
            headers/WebGPULimits.hpp
            headers/WebGPUConverters.hpp
            headers/GFX/WebGPUGFX.hpp
            headers/GFX/WebGPUSurface.hpp
            headers/GFX/WebGPURenderEncoder.hpp
            headers/GFX/WebGPUBuffer.hpp
            headers/GFX/WebGPUTexture.hpp
            headers/GFX/WebGPUSampler.hpp
            headers/GFX/WebGPUBindSet.hpp
            headers/GFX/WebGPUShaderModule.hpp
            headers/GFX/WebGPUShaderPipelineLayout.hpp
            headers/GFX/WebGPUShaderRenderPipeline.hpp
            headers/GFX/WebGPUCommandBuffer.hpp
            headers/GFX/WebGPUCommandEncoder.hpp
)