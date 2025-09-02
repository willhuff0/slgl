add_executable(SLGL_RUNTIME_EXECUTABLE)
add_executable(SLGL::RUNTIME::EXECUTABLE ALIAS SLGL_RUNTIME_EXECUTABLE)

target_sources(SLGL_RUNTIME_EXECUTABLE
    PRIVATE
        src/main.cpp
)

target_link_libraries(SLGL_RUNTIME_EXECUTABLE
    PUBLIC
        SLGL::COMMON
        SLGL::RUNTIME
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_RUNTIME_EXECUTABLE
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)
