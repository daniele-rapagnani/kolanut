#include "kolanut/graphics/vulkan/utils/Device.h"
#include "kolanut/graphics/vulkan/utils/Helpers.h"

#include <unordered_map>
#include <cassert>

namespace kola {
namespace graphics {
namespace vulkan {

Device::~Device()
{
    if (*this)
    {
        vkDestroyDevice(getVkHandle(), nullptr);
        this->handle = VK_NULL_HANDLE;
    }
}

bool Device::init(
    std::shared_ptr<PhysicalDevice> physicalDevice, 
    VkSurfaceKHR surface,
    const Config& config
)
{
    assert(!*this);
    assert(physicalDevice);

    this->physicalDevice = physicalDevice;
    this->config = config;

    this->queueGroups = {};

    for (const auto& q : config.queues)
    {
        assert(q.family);

        this->queueGroups[q.family.index].count++;
        this->queueGroups[q.family.index].configs.push_back(q);
        this->queueGroups[q.family.index].priorities.push_back(q.priority);
    }

    std::vector<VkDeviceQueueCreateInfo> dqcis = {};

    for (const auto& gr : this->queueGroups)
    {
        dqcis.emplace_back();

        dqcis.back().sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        dqcis.back().queueCount = gr.second.count;
        dqcis.back().queueFamilyIndex = gr.first;
        dqcis.back().pQueuePriorities = &gr.second.priorities[0];
    }

    for (const std::string& e : config.extensions)
    {
        if (!this->physicalDevice->isExtensionSupported(e))
        {
            knM_logFatal("Physical device '" 
                << physicalDevice->getName() 
                << "' doesn't support extension '" 
                << e << "'"
            );

            return false;
        }
    }

    std::vector<const char*> ccExtensions = toConstCharVec(config.extensions);

    VkDeviceCreateInfo dci = {};
    dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    dci.pQueueCreateInfos = &dqcis[0];
    dci.queueCreateInfoCount = dqcis.size();
    dci.enabledExtensionCount = ccExtensions.size();
    dci.ppEnabledExtensionNames = &ccExtensions[0];

    if (vkCreateDevice(physicalDevice->getVkHandle(), &dci, nullptr, &this->handle) != VK_SUCCESS)
    {
        knM_logFatal("Can't create Vulkan device from physical device: " << physicalDevice->getName());
        return false;
    }

    this->swapchain = std::make_shared<Swapchain>();

    if (!getSwapchain()->init(surface, shared_from_this(), getConfig().swapchain))
    {
        return false;
    }

    for (auto& gr : this->queueGroups)
    {
        for (size_t i = 0; i < gr.second.count; i++)
        {
            if (gr.second.configs[i].commandBuffersCount == 0)
            {
                // If an exact number of command buffers was not request,
                // we allocate one for each image in the swapchain times
                // the number of frames in fight to render.

                gr.second.configs[i].commandBuffersCount = 
                    std::max(static_cast<uint8_t>(1), this->config.framesInFlight) 
                    * this->getSwapchain()->getImageViews().size()
                ;
            }

            QueueFamily family = gr.second.configs[i].family;
            std::vector<std::shared_ptr<Queue>>& familyQueues = this->queues[family];

            std::shared_ptr<Queue> q = std::make_shared<Queue>();

            vkGetDeviceQueue(
                getVkHandle(), 
                family.index, 
                i, 
                &q->handle
            );

            if (!q->init(shared_from_this(), gr.second.configs[i]))
            {
                knM_logError("Can't initialize queue #" << i << " of type " << family.index);
                return false;
            }

            familyQueues.push_back(q);
        }
    }

    return true;
}

std::shared_ptr<Queue> Device::getQueue(QueueFamily family, uint32_t index /* = 0 */) const
{
    if (this->queues.find(family) == this->queues.end())
    {
        return nullptr;
    }

    if (this->queues.at(family).size() <= index)
    {
        return nullptr;
    }

    return this->queues.at(family)[index];
}

bool Device::addRenderPass(const RenderPass::Config& config)
{
    std::shared_ptr<RenderPass> rp = std::make_shared<RenderPass>();

    if (!rp->init(shared_from_this(), config))
    {
        return false;
    }

    this->renderPasses.push_back(rp);

    return true;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola