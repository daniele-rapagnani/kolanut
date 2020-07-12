#pragma once

#include "kolanut/events/Events.h"

#include <functional>
#include <cstdint>

namespace kola {
namespace events {

class EventSystem
{
public:
    using KeyPressedCallback = std::function<void(void)>;
    using QuitCallback = std::function<void(void)>;

public:
    virtual bool init(const Config& conf) = 0;
    virtual bool poll() = 0;
    virtual uint64_t getTimeMS() = 0;

    void setKeyPressedCallback(KeyPressedCallback cb)
    { this->keyPressedCb = cb; }

    void setQuitCallback(QuitCallback cb)
    { this->quitCb = cb; }

protected:
    KeyPressedCallback getKeyPressedCallback() const
    { return this->keyPressedCb; }

    QuitCallback getQuitCallback() const
    { return this->quitCb; }

private:
    KeyPressedCallback keyPressedCb = nullptr;
    QuitCallback quitCb = nullptr;
};

} // namespace events
} // namespace kola