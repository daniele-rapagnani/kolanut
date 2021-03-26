cmake_minimum_required(VERSION 3.7)
project(kolanut)

option(ENABLE_TRACY "Builds with Tracy profiling on" OFF)
option(ENABLE_VULKAN, "Build with Vulkan support" ON)

if (ENABLE_TRACY)
message(STATUS "Enabling Tracy support")
set(_ENABLE_TRACY ON)
endif()

if (ENABLE_VULKAN)
    message(STATUS "Building with Vulkan support")
    if (NOT EMSCRIPTEN)
        set(_ENABLE_VULKAN ON)
    else()
        message(ERROR "Vulkan is not supported when building for Emscripten")
    endif (NOT EMSCRIPTEN)
endif(ENABLE_VULKAN)

if (APPLE)
    set(CMAKE_MACOSX_RPATH 0)
    set(ENABLE_CUSTOM_COMPILER_FLAGS Off)
    add_compile_options(-mmacosx-version-min=10.12)
endif (APPLE)

set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

if (_ENABLE_VULKAN)
find_package(Vulkan REQUIRED FATAL_ERROR)
endif (_ENABLE_VULKAN)

option(BUILD_SHARED_LIBS "" OFF)
set(BUILD_SHARED_LIBS OFF)
option(BUILD_STATIC_LIBS ON)
set(BUILD_STATIC_LIBS ON)

add_compile_options(-Wno-deprecated-declarations)

if (EMSCRIPTEN)
    set(ZLIB_INCLUDE_DIR $ENV{EMSDK}/upstream/emscripten/cache/include)
    set(ZLIB_LIBRARY $ENV{EMSDK}/upstream/emscripten/cache/wasm/libz.a)

    set(BZIP2_INCLUDE_DIR $ENV{EMSDK}/upstream/emscripten/cache/include)
    set(BZIP2_LIBRARY $ENV{EMSDK}/upstream/emscripten/cache/wasm/libbz2.a)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/tinyxml2.em.in
        ${CMAKE_BINARY_DIR}/thirdparty/tinyxml2/CMakeLists.txt
    )
    
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/thirdparty/tinyxml2
    )
    
    execute_process(COMMAND make
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/thirdparty/tinyxml2
    )
else()
    option(BUILD_TINYXML2 "" ON)
    set(BUILD_TINYXML2 ON)
endif()

add_subdirectory(src/kolanut/libs/tmxparser)

option(DEBUG_GC "" OFF)
set(_DEBUG_GC OFF)
option(STRESS_GC "" OFF)
set(_STRESS_GC OFF)

add_subdirectory(src/kolanut/libs/melonscript-vm)

if (NOT EMSCRIPTEN)
add_subdirectory(src/kolanut/libs/glfw)
endif()

option(OPTION_BUILD_TESTS OFF)
add_subdirectory(src/kolanut/libs/cppfs)

add_subdirectory(src/kolanut/libs/glm)

if (EMSCRIPTEN)
option(MZ_LZMA OFF)
option(MZ_OPENSSL OFF)
option(MZ_ZSTD OFF)
endif()

add_subdirectory(src/kolanut/libs/minizip-ng)

if (_ENABLE_VULKAN)
execute_process(COMMAND src/kolanut/libs/shaderc/utils/git-sync-deps)
option(SHADERC_SKIP_TESTS "" ON)
set(SHADERC_SKIP_TESTS ON)
option(SHADERC_SKIP_EXAMPLES "" ON)
set(SHADERC_SKIP_EXAMPLES ON)
option(SHADERC_SKIP_INSTALL "" ON)
set(SHADERC_SKIP_INSTALL ON)
add_subdirectory(src/kolanut/libs/shaderc)
endif (_ENABLE_VULKAN)

add_compile_options(-Werror=incompatible-pointer-types)
add_compile_options(-Werror=implicit-function-declaration)
add_compile_options(-Werror=return-type)
add_compile_options(-Werror=int-conversion)
add_definitions(-D_USE_MATH_DEFINES)

if (_ENABLE_TRACY)
add_definitions(-DTRACY_ENABLE)
endif (_ENABLE_TRACY)

if (NOT WIN32)
add_compile_options(-Werror=macro-redefined)
endif (NOT WIN32)

add_compile_options(-Wno-double-promotion)
add_compile_options(-Wno-format)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")

if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif(EMSCRIPTEN)

configure_file(
    src/kolanut/core/BuildConfig.h.in 
    ${CMAKE_BINARY_DIR}/src/kolanut/core/BuildConfig.h
)

if (_ENABLE_VULKAN)
    set(
        VULKAN_SRC_FILES
        src/kolanut/graphics/vulkan/RendererVK.cpp
        src/kolanut/graphics/vulkan/TextureVK.cpp
        src/kolanut/graphics/vulkan/FontVK.cpp
        src/kolanut/graphics/vulkan/ShaderVK.cpp
        src/kolanut/graphics/vulkan/ProgramVK.cpp
        src/kolanut/graphics/vulkan/GeometryBufferVK.cpp

        src/kolanut/graphics/vulkan/utils/Instance.cpp
        src/kolanut/graphics/vulkan/utils/Helpers.cpp
        src/kolanut/graphics/vulkan/utils/PhysicalDevice.cpp
        src/kolanut/graphics/vulkan/utils/Device.cpp
        src/kolanut/graphics/vulkan/utils/Swapchain.cpp
        src/kolanut/graphics/vulkan/utils/ShaderModule.cpp
        src/kolanut/graphics/vulkan/utils/RenderPass.cpp
        src/kolanut/graphics/vulkan/utils/Pipeline.cpp
        src/kolanut/graphics/vulkan/utils/Pipeline2D.cpp
        src/kolanut/graphics/vulkan/utils/Queue.cpp
        src/kolanut/graphics/vulkan/utils/Semaphore.cpp
        src/kolanut/graphics/vulkan/utils/Fence.cpp
        src/kolanut/graphics/vulkan/utils/MemoryManager.cpp
        src/kolanut/graphics/vulkan/utils/CommandBuffer.cpp
        src/kolanut/graphics/vulkan/utils/Buffer.cpp
        src/kolanut/graphics/vulkan/utils/Texture.cpp
        src/kolanut/graphics/vulkan/utils/DescriptorPool.cpp
        src/kolanut/graphics/vulkan/utils/DescriptorSet.cpp
        src/kolanut/graphics/vulkan/utils/StagedBuffer.cpp
        src/kolanut/graphics/vulkan/utils/UniformsBuffer.cpp
        src/kolanut/graphics/vulkan/utils/QueryPool.cpp
    )
endif(_ENABLE_VULKAN)

set(
    SRC_FILES
    src/kolanut/stats/Stats.cpp

    src/kolanut/graphics/Graphics.cpp
    src/kolanut/graphics/Tilemap.cpp
    src/kolanut/graphics/Renderer.cpp
    src/kolanut/graphics/Shader.cpp
    src/kolanut/graphics/Program.cpp
    src/kolanut/graphics/TextureSTB.cpp
    src/kolanut/graphics/FontSTB.cpp
    src/kolanut/graphics/GeometryBuffer.cpp
    
    src/kolanut/graphics/glfw/RendererGLFW.cpp

    src/kolanut/graphics/ogl/RendererOGL.cpp
    src/kolanut/graphics/ogl/TextureOGL.cpp
    src/kolanut/graphics/ogl/FontOGL.cpp
    src/kolanut/graphics/ogl/ShaderOGL.cpp
    src/kolanut/graphics/ogl/ProgramOGL.cpp
    src/kolanut/graphics/ogl/GeometryBufferOGL.cpp
    src/kolanut/graphics/ogl/utils/shaders/Shader.cpp
    src/kolanut/graphics/ogl/utils/shaders/Program.cpp
    src/kolanut/graphics/ogl/utils/GenericUtils.cpp    
    src/kolanut/graphics/ogl/utils/Context.cpp    

    ${VULKAN_SRC_FILES}

    src/kolanut/graphics/Font.cpp
    src/kolanut/events/glfw/EventSystemGLFW.cpp
    src/kolanut/events/Events.cpp
    src/kolanut/core/Kolanut.cpp

    src/kolanut/filesystem/Filesystem.cpp
    src/kolanut/filesystem/FilesystemEngine.cpp
    src/kolanut/filesystem/files/FilesFilesystemEngine.cpp
    src/kolanut/filesystem/zips/ZipsFilesystemEngine.cpp
    
    src/kolanut/scripting/Scripting.cpp
    src/kolanut/scripting/melon/ScriptingEngineMelon.cpp
    src/kolanut/scripting/melon/ffi/OOP.cpp
    
    src/kolanut/scripting/melon/bindings/Camera.cpp
    src/kolanut/scripting/melon/bindings/Texture.cpp
    src/kolanut/scripting/melon/bindings/Bindings.cpp
    src/kolanut/scripting/melon/bindings/Tilemap.cpp
    src/kolanut/scripting/melon/bindings/Font.cpp
    src/kolanut/scripting/melon/bindings/Sound.cpp
    src/kolanut/scripting/melon/bindings/SoundInstance.cpp
    src/kolanut/scripting/melon/bindings/Transform3D.cpp
    src/kolanut/scripting/melon/bindings/Tracy.cpp

    src/kolanut/scripting/melon/modules/KolanutModule.cpp
    src/kolanut/scripting/melon/modules/SourceModule.cpp
    src/kolanut/scripting/melon/modules/Vector2Module.cpp
    src/kolanut/scripting/melon/modules/Vector3Module.cpp
    src/kolanut/scripting/melon/modules/Vector4Module.cpp
    src/kolanut/scripting/melon/modules/BoundingboxModule.cpp
    src/kolanut/scripting/melon/modules/RectModule.cpp
    src/kolanut/scripting/melon/modules/ColorModule.cpp

    src/kolanut/audio/Audio.cpp
    src/kolanut/audio/miniaudio/MiniaudioAudioEngine.cpp
    src/kolanut/audio/miniaudio/MiniaudioSoundInstance.cpp

    src/kolanut/libs/melonscript-vm/src/melon/tools/utils.cpp
    src/kolanut/libs/tracy/TracyClient.cpp

    src/kolanut/main.cpp
)

if (NOT EMSCRIPTEN)
set(SRC_FILES ${SRC_FILES} src/kolanut/graphics/ogl/glad/src/glad.c)
endif()

add_executable(kolanut ${SRC_FILES})

set(
    KOLANUT_INCLUDE_DIRS

    src/
    ${CMAKE_BINARY_DIR}/src/
    src/kolanut/libs/linalg/
    src/kolanut/libs/stb/
    src/kolanut/graphics/ogl/glad/include/
    src/kolanut/libs/cppfs/source/cppfs/include/
    ${cppfs_BINARY_DIR}/source/cppfs/include/
    src/kolanut/libs/minizip-ng/
    src/kolanut/libs/tracy/
    src/kolanut/libs/miniaudio/
    src/kolanut/libs/inipp/
)

if (_ENABLE_VULKAN)
set(
    KOLANUT_INCLUDE_DIRS
    ${KOLANUT_INCLUDE_DIRS}
    src/kolanut/libs/shaderc/libshaderc/include/
    ${Vulkan_INCLUDE_DIR}
)
endif (_ENABLE_VULKAN)

target_include_directories(kolanut PUBLIC ${KOLANUT_INCLUDE_DIRS})

if (MINGW)
    # On MinGW add static linking of system libraries so that the resulting
    # executable is portable.
    # Also use lld to link the binaries: this solves some issues with catch2
    # and also speeds the build up considerably.
    
    set(
        MINGW_LD_FLAGS 
        "-static-libgcc" 
        "-static-libstdc++" 
        "-static" "-lpthread" 
        "-static" "-lz" 
        "-fuse-ld=lld"
    )

    target_link_options(kolanut PUBLIC ${MINGW_LD_FLAGS})
endif(MINGW)

set(LINKLIBS 
    melonscript-vm 
    external 
    cjson 
    glfw 
    glm 
    tmxparser 
    cppfs 
    minizip
)

if (ENABLE_VULKAN)
set(
    LINKLIBS 
    ${LINKLIBS}
    shaderc 
    ${Vulkan_LIBRARIES}
)
endif()

if (EMSCRIPTEN)
target_link_options(
    kolanut 
    PUBLIC 
    "SHELL:-s USE_GLFW=3" 
    "SHELL:-s USE_ZLIB=1"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    "SHELL:-s DEMANGLE_SUPPORT=1"
    "SHELL:-g2"
    "SHELL:-g3"
    "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/tests/@/"
)
endif()

target_link_libraries(kolanut ${LINKLIBS})

target_include_directories(
    kolanut PUBLIC     
    src/
    ${PROJECT_BINARY_DIR}
)