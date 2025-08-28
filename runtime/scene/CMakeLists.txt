add_library(SLGL_RUNTIME_SCENE)
add_library(SLGL::RUNTIME::SCENE ALIAS SLGL_RUNTIME_SCENE)

target_sources(SLGL_RUNTIME_SCENE
    PRIVATE
        src/Scene.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/runtime/scene/Scene.hpp
)

target_link_libraries(SLGL_RUNTIME_SCENE
    PUBLIC
        SLGL::RUNTIME::GRAPHICS
        SLGL::RUNTIME::RESOURCE_READER
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_RUNTIME_SCENE
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)