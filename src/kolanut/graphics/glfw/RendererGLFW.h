#pragma once

#include "kolanut/graphics/Renderer.h"

#include <GLFW/glfw3.h>

#include <memory>

namespace kola {
namespace graphics {

class RendererGLFW : public Renderer
{ 
public:
    virtual ~RendererGLFW();

public:
    virtual bool doInit(const Config& config) override;
    void doFlip() override;

    Sizei getResolution() const override;
    Sizei getPixelResolution() const override;

    GLFWwindow* getWindow() const
    { return this->window; }

    virtual void onUpdateWindowSize() { };

    void setUseNoAPI(bool val)
    { this->noAPI = val; }

    bool isUseNoAPI() const
    { return this->noAPI; }

private:
    GLFWwindow* window = {};
    bool noAPI = false;
};

} // namespace graphics
} // namespace kola