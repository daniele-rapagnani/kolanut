#pragma once

#include "kolanut/graphics/vulkan/utils/VkHandle.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <functional>
#include <cstdint>

namespace kola {
namespace graphics {
namespace vulkan {

class Instance : public VkHandle<VkInstance>
{
public:
    using MessengerCb = std::function<
        void(
            VkDebugUtilsMessageSeverityFlagBitsEXT,
            VkDebugUtilsMessageTypeFlagsEXT,
            const std::string& message
        )
    >;

    struct Config
    {
        std::string appName = {};
        std::vector<std::string> extensions = {};
        std::vector<std::string> layers = {};
        uint32_t apiVersion = VK_API_VERSION_1_0;
        MessengerCb messengerCb = {};
    };

private:
    static std::vector<VkExtensionProperties> supportedExtensions;

public:
    static const std::vector<VkExtensionProperties>& getSupportedExtensions();
    static bool isExtensionSupported(const std::string& extension);

public:
    ~Instance();

    bool init(const Config& config);

    const Config& getConfig()
    { return this->config; }

private:
    Config config = {};
    VkDebugUtilsMessengerEXT messenger = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola