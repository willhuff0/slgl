add_library(SLGL_RUNTIME_RESOURCES)
add_library(SLGL::RUNTIME::RESOURCES ALIAS SLGL_RUNTIME_RESOURCES)

target_sources(SLGL_RUNTIME_RESOURCES
    PRIVATE
        src/Resources.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/runtime/resources/Resource.hpp
            include/slgl/runtime/resources/ResourceLoader.hpp
            include/slgl/runtime/resources/GetResource.hpp
            include/slgl/runtime/resources/ResourceGenerator.hpp
)

target_link_libraries(SLGL_RUNTIME_RESOURCES
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_RUNTIME_RESOURCES
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)