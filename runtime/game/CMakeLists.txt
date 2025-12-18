include(FetchContent)

add_library(SLGL_RUNTIME_GAME)
add_library(SLGL::RUNTIME::GAME ALIAS SLGL_RUNTIME_GAME)

target_sources(SLGL_RUNTIME_GAME
    PRIVATE
        src/Game.cpp
        src/Provider.cpp
        src/utils/RNG.cpp
        src/TempImageLoader.cpp
        src/FlyCamera.cpp
        src/Terrain.cpp
        src/Skybox.cpp
        src/Node.cpp
        src/PostProcessing.cpp
        src/Vehicle.cpp
        src/MeshRenderer.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/slgl/runtime/game/deps/FastNoiseLite.h
            include/slgl/runtime/game/deps/stb_image.h
            include/slgl/runtime/game/Game.hpp
            include/slgl/runtime/game/Provider.hpp
            include/slgl/runtime/game/utils/Paths.hpp
            include/slgl/runtime/game/utils/RNG.hpp
            include/slgl/runtime/game/TempImageLoader.hpp
            include/slgl/runtime/game/FlyCamera.hpp
            include/slgl/runtime/game/Terrain.hpp
            include/slgl/runtime/game/Skybox.hpp
            include/slgl/runtime/game/PostProcessing.hpp
            include/slgl/runtime/game/Node.hpp
            include/slgl/runtime/game/Vehicle.hpp
            include/slgl/runtime/game/MeshRenderer.hpp
)

FetchContent_Declare(decs
    GIT_REPOSITORY https://github.com/willhuff0/decs
    GIT_TAG        5e2a952a0cc050f15bfe3c0bf40bf3154fdaedae
    GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(decs)

find_package(assimp REQUIRED)

target_link_libraries(SLGL_RUNTIME_GAME
    PUBLIC
        SLGL::COMMON
        SLGL::RUNTIME
        decs
        assimp::assimp
    PRIVATE
        slgl_warnings
)

target_include_directories(SLGL_RUNTIME_GAME
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>
)
