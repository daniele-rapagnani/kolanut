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
    Recti getViewport() const override;

    GLFWwindow* getWindow() const
    { return this->window; }

    virtual void onUpdateWindowSize()
    { 
        updateViewport();
    };

    void setUseNoAPI(bool val)
    { this->noAPI = val; }

    bool isUseNoAPI() const
    { return this->noAPI; }

    void updateViewport(Sizei newScreenSize);
    void updateViewport();

private:
    GLFWwindow* window = {};
    bool noAPI = false;
    Recti viewport = {};
};

} // namespace graphics
} // namespace kola