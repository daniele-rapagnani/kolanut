# Kolanut

**Table of contents:**
* [What's this](#whats-this)
* [Why](#why)
* [Building](#building)
* [Running](#running)
* [Boot script](#boot-script)
* [Configuration](#configuration)

## What's this?

Kolanut is a simple game engine that can be used to make **2D games** in [melon](https://github.com/daniele-rapagnani/melon).

Some of Kolanut's current **features** are:
* Custom **OpenGL** 2.1/ES and **Vulkan** renderer
* Basic **audio** with [miniaudio](https://github.com/mackron/miniaudio)
* Can ran in the **browser** thanks to [Emscripten](https://emscripten.org/)
* Supports **tilemaps** with [Tiled](https://www.mapeditor.org/)
* **Simple low-level API**: it doesn't shove a particular engine's architecture down your throat.
* Support for **archive based filesystem** makes it easy to distribute your game and import pre-built modules.
* Supports **profiling** through [Tracy](https://github.com/wolfpld/tracy)

## Why

Kolanut was mainly made as a toy project to test the [melon](https://github.com/daniele-rapagnani/melon) programming language and experiment with different graphic APIs and techniques. 

With time I found Kolanut to be really fun to work with and I decided to promote it to a project of its own.

Kolanut's philosophy is to expose only a small set of low-level functions to melon, delegating it all the rest of the work. While this decision was mainly made as a result of the need to road test melon, it made the engine very fun to work with as it can scale with your own needs. You can just draw some simple sprites on the screen or use a pre-made engine such as [kolanut-ecs](https://github.com/daniele-rapagnani/kolanut-ecs) to make a full-fledged game.

## Building

Make sure to initialize all submodules after cloning the repo:

```sh
git submodule update --init --recursive
```

All dependencies are included except for the Vulkan SDK.
If you want to enable Vulkan you must install the [SDK](https://vulkan.lunarg.com/sdk/home) on your own.

On all platforms, if you are building with Vulkan you should fetch [shaderc](https://github.com/google/shaderc) dependencies first by running the following in bash/msys:

```sh
cd src/kolanut/libs/shaderc
utils/git-sync-deps
```

Kolanut uses CMake so you can use the standard way of building CMake projects:

```sh
mkdir build && cd build
cmake ..
make
```

There are some extra build options that can be enabled or disabled:

| Option        | Description                               |
|---------------|-------------------------------------------|
| ENABLE_VULKAN | Builds with Vulkan rendering support      |
| ENABLE_TRACY  | Enables profiling with Tracy              |

### Platform specific notes

### Windows

Kolanut currently supports MinGW-w64 only, you can simply build it with
CMake from an MSYS2 shell, make sure to use the `MSYS Makefiles` generator and
to correctly set the `CMAKE_INSTALL_PREFIX`, something like this:

```sh
cmake <your args> -DCMAKE_INSTALL_PREFIX="/usr/local/" -G "MSYS Makefiles"
```

### Emscripten

Start a shell session where emscripten's is available and pre-install
the needed dependencies:

```sh
embuilder build zlib bzip2
```

To build use the standard CMake flow, just run CMake with `emcmake`:

```sh
mkdir build && cd build
emcmake cmake <your args> ..
emmake make
```

## Running

If you run Kolanut's executable in the current directory, by default, Kolanut will:
* Load the game's configuration from `./scripts/config.ms`
* Run the game by running `./scripts/boot.ms`

## Boot script

Kolanut will assume that certain functions are defined by your boot script as they will be used by the engine
to pass informations or trigger events. 
If you don't define one or more of them a default will be used (which usually does nothing).

These functions are:

### `Kolanut.onLoad()`

This function is called when the engine is initialized and can be used
to load the resources needed by the game to start.

### `Kolanut.onUpdate(dt)`

This function is called every frame when the update phase starts.
The current delta time is passed as the first argument

### `Kolanut.onDraw()`

This function is called every frame when the drawing phase starts.

### `Kolanut.onDrawUI()`

This function is called every frame when the UI drawin phase starts.
In this phase the camera's transform is automatically reset.

### `Kolanut.onStatsUpdated(result)`

This function is called everytime there's new stats data available if
stats are enabled in the configuration.

The `result` argument will be populated with an object with the following keys:

| Key | Description |
| --- | ----------- |
| label | A meaningful name for the measure |
| avg | The average value of this measure in the last 60 frames |
| total | The total value of this measure int he last 60 frames |
| samplesCount | The number of samples for this measure (usually 60) |

### `Kolanut.onQuit()`

This function will be called when the user tries to close the game's window.
If `false` is returned then the game will prevent the window from closing.
If this function is not specified, a default one will be used which always returns `true`.

### `Kolanut.onKeyPressed(keyName, pressed)`

This function will be called everytime a key is pressed.
The `keyName` argument contains a string representing the name of the pressed key, `pressed` is a boolean
with the value `true` if the key has been pressed and `false` if it has been released.

### Example boot script

```js
Kolanut.onLoad = => {
    // Load your assets here
};

Kolanut.onUpdate = |dt| => {
    // Update stuff
};

Kolanut.onDraw = => {
    // Draw stuff
};
```

## Configuration

Kolanut will initialize essential subsystems first (such as the scripting and filesystem subsystems) before the other ones. This means that there are two ways to configure the engine: essential subsystems are configured via an INI configuration file (`bootstrap.ini`) and the rest of the subsystems are configured by running a melon script (`./scripts/config.ms`).

### `bootstrap.ini`

You can place a `boostrap.ini` file in the root of your project to initialize essential subsystems.
This file can't be placed inside a `.nut` archive and should physically reside on the filesystem.

Supported values are:

| Section | Key | Description | Values | Default |
|---------|-----|-------------|--------|---------|
| filesystem | root | The directory in which all data of your game is located | a string | `.` |
| filesystem | engine | The filesystem engine to use | `zips`, `files` | `zips` |
| scripting | boot_script | The first melon script that will be run | a string | `boot` |
| scripting | config_script | The script that will be run to import the configuration | a string | `config` |
| scripting | scripts_root_dir | The directory, inside the root directory, that holds your scripts | a string | `scripts` |

An example of `boostrap.ini` file:

```ini
[filesystem]
root=./assets
engine=files

[scripting]
scripts_root_dir=.
```

### `config.ms`

The `config.ms` script should return an object containing the configuration, for example:

```js
let designRes = Vector2.create(400, 300);

return {
    windowTitle = "My Game",
    screenSize = designRes * 2,
    designResolution = designRes,
    graphicBackend = "opengl"
};
```

Supported values are:

| Key | Description | Values  | Default |
|-----|-------------|---------|---------|
| graphicBackend | The graphic API to use | `"opengl"`, `"vulkan"` (if enabled) | `"opengl"` |
| eventSystem | The event/input system to use | `"glfw"` | `"glfw"` |
| resolutionFitMode | How to adjust the game's resolution if the window is of a different size | `"contain"`, `"cover"`, `"stretch"`  | `"contain"` |
| windowTitle | The title of game's window if windowed | a string | `"Kolanut"` |
| screenSize | The window size if the game runs windowed | a `Vector2` | `Vector2.create(800, 600)` |
| designResolution | The actual resolution the game will use | a `Vector2` | `Vector2.create(800, 600)` |
| fullscreen | Whether the game should run fullscreen | a boolean | `false` |
| vsync | Whether the game should be v-synced (platform dependent) | a boolean | `true` |
| enableGraphicAPIDebug | This enables validation layers in Vulkan | a boolean | `false` |
| framesInFlight | The number of total frames to pre-render without syncing with the GPU | an integer | `2` |
| maxGeometryBufferVertices | The maximum number of vertices that can be displayed at the sime time in a frame | an integer | `120000` |
| jobQueueInitialSize | The initial size of the render queue | an integer | `5000` |
| forceGPU | If multiple GPUs are available, force one by name | a string | `""` |
| enableStats | Display stats about the game's performance | a boolean | `false` |
| mainVertexShaderPath | The path to the main vertex shader | a string | `"main.vert"` |
| mainFragmentShaderPath | The path to the main fragment shader | a string | `"main.frag"` |
| lineVertexShaderPath | The path to the vertex shader used to draw vector polygons | a string | `"main.vert"` |
| lineFragmentShaderPath | The path to the fragment shader used to draw vector polygons | a string | `"solid.frag"` |