#include "kolanut/graphics/vulkan/utils/Helpers.h"

#include <string>

namespace kola {
namespace graphics {
namespace vulkan {

std::vector<const char*> toConstCharVec(const std::vector<std::string>& sv)
{
    std::vector<const char*> ccv;
    ccv.reserve(sv.size());

    for (const std::string& s : sv)
    {
        ccv.push_back(s.c_str());
    }

    return ccv;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola