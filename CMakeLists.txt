cmake_minimum_required(VERSION 3.7)
project(kolanut)

if (APPLE)
    set(CMAKE_MACOSX_RPATH 0)
    set(ENABLE_CUSTOM_COMPILER_FLAGS Off)
endif (APPLE)

set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

option(DEBUG_GC "" OFF)
set(DEBUG_GC OFF)
option(STRESS_GC "" OFF)
set(STRESS_GC OFF)

add_subdirectory(src/kolanut/libs/melonscript-vm)

option(SDL_STATIC "" ON)
option(SDL_SHARED "" ON)
set(SDL_STATIC ON)
set(SDL_SHARED OFF)
add_subdirectory(src/kolanut/libs/SDL)

option(BUILD_SHARED_LIBS "" OFF)
set(BUILD_SHARED_LIBS OFF)
option(BUILD_TINYXML2 "" ON)
set(BUILD_TINYXML2 ON)
add_subdirectory(src/kolanut/libs/tmxparser)

add_definitions(-Werror=incompatible-pointer-types)
add_definitions(-Werror=implicit-function-declaration)
add_definitions(-Werror=return-type)
add_definitions(-Werror=int-conversion)
add_definitions(-Werror=macro-redefined)
add_definitions(-Wdouble-promotion)
add_definitions(-Wno-format)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")

set(
    SRC_FILES
    src/kolanut/graphics/Graphics.cpp
    src/kolanut/graphics/Tilemap.cpp
    src/kolanut/graphics/Renderer.cpp
    src/kolanut/graphics/sdl/RendererSDL.cpp
    src/kolanut/graphics/sdl/TextureSDL.cpp
    src/kolanut/graphics/Font.cpp
    src/kolanut/graphics/sdl/FontSDL.cpp
    src/kolanut/events/Events.cpp
    src/kolanut/events/sdl/EventSystemSDL.cpp
    src/kolanut/core/Kolanut.cpp

    src/kolanut/filesystem/Filesystem.cpp
    src/kolanut/filesystem/FilesystemEngine.cpp
    src/kolanut/filesystem/files/FilesFilesystemEngine.cpp
    
    src/kolanut/scripting/Scripting.cpp
    src/kolanut/scripting/melon/ScriptingEngineMelon.cpp
    src/kolanut/scripting/melon/ffi/OOP.cpp
    src/kolanut/scripting/melon/bindings/Camera.cpp
    src/kolanut/scripting/melon/bindings/Texture.cpp
    src/kolanut/scripting/melon/bindings/Bindings.cpp
    src/kolanut/scripting/melon/bindings/Tilemap.cpp
    src/kolanut/scripting/melon/bindings/Font.cpp
    src/kolanut/scripting/melon/modules/KolanutModule.cpp
    src/kolanut/scripting/melon/modules/SourceModule.cpp
    src/kolanut/scripting/melon/modules/VectorModule.cpp
    src/kolanut/scripting/melon/modules/SpriteModule.cpp
    src/kolanut/scripting/melon/modules/RectModule.cpp
    src/kolanut/scripting/melon/modules/ColorModule.cpp
    src/kolanut/libs/melonscript-vm/src/melon/tools/utils.cpp

    src/kolanut/main.cpp
)

add_executable(
    kolanut
    ${SRC_FILES}
)

target_include_directories(
    kolanut PUBLIC
    src/
    src/kolanut/libs/linalg/
    src/kolanut/libs/stb/
)

set(LINKLIBS melonscript-vm external cjson SDL2-static tmxparser)

target_link_libraries(kolanut ${LINKLIBS})
