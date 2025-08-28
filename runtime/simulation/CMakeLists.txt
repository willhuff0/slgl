add_library(SLGL_RUNTIME_SIMULATION)
add_library(SLGL::RUNTIME::SIMULATION ALIAS SLGL_RUNTIME_SIMULATION)

target_sources(SLGL_RUNTIME_SIMULATION
    PRIVATE
        src/Simulation.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/runtime/simulation/Simulation.hpp
)

target_link_libraries(SLGL_RUNTIME_SIMULATION
    PUBLIC
        SLGL::RUNTIME::SCENE
        SLGL::RUNTIME::PHYSICS
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_RUNTIME_SIMULATION
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)