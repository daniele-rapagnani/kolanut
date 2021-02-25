
#pragma once

#include "kolanut/core/Logging.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <cassert>

namespace kola {
namespace graphics {
namespace vulkan {

struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

template <typename T>
VkResult getVulkanList(
    std::vector<T>& out,
    std::function<VkResult(uint32_t* count, T* list)> listFunc
)
{
    uint32_t count = 0;
    VkResult r = VK_SUCCESS;

    if ((r = listFunc(&count, nullptr)) != VK_SUCCESS)
    {
        return r;
    }

    out.clear();
    out.resize(count);

    if (count == 0)
    {
        return VK_SUCCESS;
    }

    if ((r = listFunc(&count, &out[0])) != VK_SUCCESS)
    {
        return r;
    }

    return VK_SUCCESS;
}

template <typename T>
std::vector<T> getVulkanList(
    std::function<void(uint32_t* count, T* list)> listFunc
)
{
    std::vector<T> out;
    
    getVulkanList<T>(out, [listFunc] (uint32_t* count, T* list) {
        listFunc(count, list);
        return VK_SUCCESS;
    });

    return out;
}

template <typename T>
std::vector<T> getVulkanListFatal(
    std::function<VkResult(uint32_t* count, T* list)> listFunc,
    const std::string& error
)
{
    std::vector<T> out;
    VkResult errorCode = getVulkanList<T>(out, listFunc);

    if (errorCode != VK_SUCCESS)
    {
        knM_logFatal(error << ". Code: " << errorCode);
    }

    return out;
}

template <typename T, typename K>
bool testFlag(T flag, K bit)
{
    return (flag & bit) == bit;
}

std::vector<const char*> toConstCharVec(const std::vector<std::string>& sv);

template <typename T, typename ...Args>
std::shared_ptr<T> make_init(Args... args)
{
    std::shared_ptr<T> inst = std::make_shared<T>();

    if (!inst)
    {
        return nullptr;
    }

    if (!inst->init(std::forward<Args>(args)...))
    {
        return nullptr;
    }

    return inst;
}

template <typename T, typename ...Args>
std::shared_ptr<T> make_init_fatal(Args... args)
{
    std::shared_ptr<T> i = make_init<T>(std::forward<Args>(args)...);
    assert(i);
    
    return i;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola