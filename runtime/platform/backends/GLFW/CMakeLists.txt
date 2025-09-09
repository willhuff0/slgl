include(FetchContent)

find_package(glfw3 3.4 REQUIRED)
target_link_libraries(SLGL_RUNTIME_PLATFORM PRIVATE glfw)

target_sources(SLGL_RUNTIME_PLATFORM
    PRIVATE
        src/GLFWBackend.cpp
        src/GLFWWindow.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/runtime/platform/backends/GLFW/GLFWBackend.hpp
            include/slgl/runtime/platform/backends/GLFW/GLFWWindow.hpp
)

target_include_directories(SLGL_RUNTIME_PLATFORM
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
    PRIVATE
        include/slgl/runtime/platform
)