find_package(SDL3 3.2.22 REQUIRED)
target_link_libraries(SLGL_RUNTIME_PLATFORM PRIVATE SDL3::SDL3)

target_sources(SLGL_RUNTIME_PLATFORM
        PRIVATE
            src/SDL3Backend.cpp
            src/SDL3Window.cpp
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/runtime/platform/backends/SDL3/SDL3Backend.hpp
            include/slgl/runtime/platform/backends/SDL3/SDL3Window.hpp
)

target_include_directories(SLGL_RUNTIME_PLATFORM
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
    PRIVATE
        include/slgl/runtime/platform
)