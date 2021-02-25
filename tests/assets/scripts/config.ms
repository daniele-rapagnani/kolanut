let screenSize = Vector.create(800, 600);

return {
    windowTitle = "Yeah",
    screenSize = screenSize,
    designResolution = screenSize * 0.5,
    fullscreen = false,
    graphicBackend = "vulkan",
    enableGraphicAPIDebug = false,
    framesInFlight = null,
    maxGeometryBufferVertices = null
};