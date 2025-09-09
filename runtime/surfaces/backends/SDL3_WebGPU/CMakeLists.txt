include(FetchContent)

find_package(SDL3 3.2.22 REQUIRED)
FetchContent_Declare(sdl3webgpu
    URL      https://github.com/eliemichel/sdl3webgpu/archive/refs/heads/main.tar.gz
    URL_HASH SHA256=67fb46e03aec005f67d3784f5cafbaf7210837f0c1ea56b4edac539bc9356d44
)
FetchContent_MakeAvailable(sdl3webgpu)
target_link_libraries(SLGL_RUNTIME_SURFACES PRIVATE sdl3webgpu)

target_sources(SLGL_RUNTIME_SURFACES
    PRIVATE
        src/SDL3_WebGPU_SurfaceCreator.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS headers
        FILES
            headers/backends/SDL3_WebGPU_SurfaceCreator.hpp
)

target_include_directories(SLGL_RUNTIME_SURFACES PRIVATE headers)