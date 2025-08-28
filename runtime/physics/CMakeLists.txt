add_library(SLGL_RUNTIME_PHYSICS)
add_library(SLGL::RUNTIME::PHYSICS ALIAS SLGL_RUNTIME_PHYSICS)

target_sources(SLGL_RUNTIME_PHYSICS
    PRIVATE
        src/Physics.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/runtime/physics/Physics.hpp
)

target_link_libraries(SLGL_RUNTIME_PHYSICS
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_RUNTIME_PHYSICS
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)