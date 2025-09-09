include(FetchContent)

find_package(glfw3 3.4 REQUIRED)
FetchContent_Declare(glfw3webgpu
    URL      https://github.com/eliemichel/glfw3webgpu/archive/refs/tags/v1.3.0-alpha.tar.gz
    URL_HASH SHA256=608386a158a22636756dd44932f25675a68f190552481f48d45d63573edd5f2b
)
FetchContent_MakeAvailable(glfw3webgpu)
target_link_libraries(SLGL_RUNTIME_SURFACES PRIVATE glfw3webgpu)

target_sources(SLGL_RUNTIME_SURFACES
    PRIVATE
        src/GLFW_WebGPU_SurfaceCreator.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS headers
        FILES
            headers/backends/GLFW_WebGPU_SurfaceCreator.hpp
)

target_include_directories(SLGL_RUNTIME_SURFACES PRIVATE headers)