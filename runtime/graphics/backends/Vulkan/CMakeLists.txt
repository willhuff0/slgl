find_package(Vulkan)
target_link_libraries(SLGL_RUNTIME_GRAPHICS PRIVATE Vulkan::Vulkan)

target_compile_definitions(SLGL_RUNTIME_GRAPHICS
    PUBLIC
        VULKAN_HPP_NO_CONSTRUCTORS
)

target_sources(SLGL_RUNTIME_GRAPHICS
    PRIVATE
        src/VulkanBackend.cpp
        src/VulkanContext.cpp
        src/VulkanBuffer.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/runtime/graphics/backends/Vulkan/VulkanBackend.hpp
            include/slgl/runtime/graphics/backends/Vulkan/VulkanContext.hpp
            include/slgl/runtime/graphics/backends/Vulkan/VulkanBuffer.hpp
)

target_include_directories(SLGL_RUNTIME_GRAPHICS
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
    PRIVATE
        include/slgl/runtime/graphics
)