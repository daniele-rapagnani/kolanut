#pragma once

#include "kolanut/events/EventSystem.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

namespace kola {
namespace events {

class EventSystemGLFW : public EventSystem
{ 
public:
    ~EventSystemGLFW();

    bool init(const Config& conf) override;
    bool poll() override;
    uint64_t getTimeMS() override;
    bool isKeyPressed(KeyCode key) override;

protected:
    void processKey(int key, int action);

protected:
    bool keysPressedMap[static_cast<unsigned int>(KeyCode::KEY_COUNT)] = {0};
    GLFWwindow* window = {};
};

} // namespace events
} // namespace kola