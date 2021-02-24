cmake_minimum_required(VERSION 3.7)
project(kolanut)

if (APPLE)
    set(CMAKE_MACOSX_RPATH 0)
    set(ENABLE_CUSTOM_COMPILER_FLAGS Off)
endif (APPLE)

set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

find_package(Vulkan REQUIRED FATAL_ERROR)

option(DEBUG_GC "" OFF)
set(DEBUG_GC OFF)
option(STRESS_GC "" OFF)
set(STRESS_GC OFF)

add_subdirectory(src/kolanut/libs/melonscript-vm)

option(BUILD_SHARED_LIBS "" OFF)
set(BUILD_SHARED_LIBS OFF)
option(BUILD_TINYXML2 "" ON)
set(BUILD_TINYXML2 ON)
add_subdirectory(src/kolanut/libs/tmxparser)

add_subdirectory(src/kolanut/libs/glfw)
add_subdirectory(src/kolanut/libs/glm)

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
    
    src/kolanut/graphics/ogl/glad/src/glad.c
    src/kolanut/graphics/ogl/RendererOGL.cpp
    src/kolanut/graphics/ogl/TextureOGL.cpp
    src/kolanut/graphics/ogl/FontOGL.cpp
    src/kolanut/graphics/ogl/utils/shaders/Shader.cpp
    src/kolanut/graphics/ogl/utils/shaders/Program.cpp
    src/kolanut/graphics/ogl/utils/GenericUtils.cpp

    src/kolanut/graphics/vulkan/RendererVK.cpp
    src/kolanut/graphics/vulkan/TextureVK.cpp
    src/kolanut/graphics/vulkan/FontVK.cpp
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
    src/kolanut/graphics/vulkan/utils/GeometryBuffer.cpp
    src/kolanut/graphics/vulkan/utils/UniformsBuffer.cpp
    src/kolanut/graphics/vulkan/utils/QueryPool.cpp

    src/kolanut/graphics/Font.cpp
    src/kolanut/events/glfw/EventSystemGLFW.cpp
    src/kolanut/events/Events.cpp
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
    src/kolanut/graphics/ogl/glad/include/
    ${Vulkan_INCLUDE_DIR}
)

set(LINKLIBS melonscript-vm external cjson glfw glm tmxparser ${Vulkan_LIBRARIES})

target_link_libraries(kolanut ${LINKLIBS})
