include(FetchContent)

if(WIN32)
    set(GLFW_BUILD_WIN32 ON CACHE BOOL "" FORCE)
elseif(APPLE)
    set(GLFW_BUILD_COCOA ON CACHE BOOL "" FORCE)
elseif(UNIX)
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
endif()

if (GLFW_BUILD_COCOA)
    target_compile_definitions(SLGL_RUNTIME_WSI PRIVATE _GLFW_COCOA)
    target_compile_options(SLGL_RUNTIME_WSI PRIVATE -x objective-c)
    target_link_libraries(SLGL_RUNTIME_WSI PRIVATE "-framework Cocoa" "-framework CoreVideo" "-framework IOKit" "-framework QuartzCore")
endif()
if (GLFW_BUILD_WIN32)
    target_compile_definitions(SLGL_RUNTIME_WSI PRIVATE _GLFW_WIN32)
endif()
if (GLFW_BUILD_X11)
    target_compile_definitions(SLGL_RUNTIME_WSI PRIVATE _GLFW_X11)
endif()
if (GLFW_BUILD_WAYLAND)
    target_compile_definitions(SLGL_RUNTIME_WSI PRIVATE _GLFW_WAYLAND)
endif()

find_package(glfw3 3.4 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
target_link_libraries(SLGL_RUNTIME_WSI
    PRIVATE
        glfw
        Vulkan::Vulkan
)

target_sources(SLGL_RUNTIME_WSI
    PRIVATE
        src/GLFW_Vulkan_WSI.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS headers
        FILES
            headers/backends/GLFW_Vulkan_WSI.hpp
)

target_include_directories(SLGL_RUNTIME_WSI PRIVATE headers)