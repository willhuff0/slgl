add_library(SLGL_RUNTIME INTERFACE)
add_library(SLGL::RUNTIME ALIAS SLGL_RUNTIME)

add_subdirectory(executable)
add_subdirectory(platform)
add_subdirectory(graphics)
add_subdirectory(physics)
add_subdirectory(resource_reader)
add_subdirectory(scene)
add_subdirectory(simulation)

target_link_libraries(SLGL_RUNTIME
    INTERFACE
        SLGL::RUNTIME::PLATFORM
        SLGL::RUNTIME::GRAPHICS
        SLGL::RUNTIME::PHYSICS
        SLGL::RUNTIME::RESOURCE_READER
        SLGL::RUNTIME::SCENE
        SLGL::RUNTIME::SIMULATION
)
