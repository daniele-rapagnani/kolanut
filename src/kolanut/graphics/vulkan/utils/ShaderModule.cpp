#include "kolanut/graphics/vulkan/utils/ShaderModule.h"
#include "kolanut/graphics/vulkan/utils/Device.h"

#include <shaderc/shaderc.hpp>

#include <unordered_map>
#include <cstdint>

namespace kola {
namespace graphics {
namespace vulkan {

namespace {
constexpr uint32_t SPIRV_BIN_MAGIC = 0x07230203;
}

ShaderModule::~ShaderModule()
{
    if (*this)
    {
        vkDestroyShaderModule(this->device->getVkHandle(), getVkHandle(), nullptr);
        this->handle = VK_NULL_HANDLE;
    }
}

bool ShaderModule::init(
    const Config& config,
    std::shared_ptr<Device> device,
    const std::string& file,
    const std::vector<char>& data
)
{
    this->config = config;
    this->fileName = file;
    this->device = device;

    if (data.size() > 4)
    {
        uint32_t magicHeader = *reinterpret_cast<const uint32_t*>(&data[0]);

        if (magicHeader != SPIRV_BIN_MAGIC)
        {
            std::vector<uint32_t> code = {};

            if (!compile(data, code))
            {
                return false;
            }

            return create(code.data(), code.size() * sizeof(uint32_t));
        }
    }

    return create(reinterpret_cast<const uint32_t*>(data.data()), data.size());
}

namespace {

template <typename T>
struct enum_hasher
{
    size_t operator()(const T& t) const
    {
        return std::hash<int64_t>()(static_cast<int64_t>(t));
    }
};

std::unordered_map<ShaderModule::Type, shaderc_shader_kind, enum_hasher<ShaderModule::Type>> TYPES_MAP = {
    { ShaderModule::Type::FRAGMENT, shaderc_fragment_shader },
    { ShaderModule::Type::VERTEX, shaderc_vertex_shader }
};

}

bool ShaderModule::compile(const std::vector<char>& source, std::vector<uint32_t>& code)
{
    if (TYPES_MAP.find(getConfig().type) == TYPES_MAP.end())
    {
        return false;
    }
    
    this->errors.clear();

    shaderc::Compiler compiler = {};
    shaderc::CompileOptions options = {};

    shaderc_shader_kind kind = TYPES_MAP.at(getConfig().type);

    shaderc::SpvCompilationResult result = 
        compiler.CompileGlslToSpv(
            source.data(),
            source.size(),
            kind,
            this->fileName.c_str(),
            getConfig().entryPoint.c_str(),
            options
        )
    ;

    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        this->errors = result.GetErrorMessage();
        return false;
    }

    code.clear();
    code.reserve(std::distance(result.cbegin(), result.cend()));
    std::copy(result.cbegin(), result.cend(), std::back_inserter(code));

    return true;
}

bool ShaderModule::create(const uint32_t* code, size_t size)
{
    VkShaderModuleCreateInfo smci = {};
    smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    smci.codeSize = size;
    smci.pCode = code;

    if (vkCreateShaderModule(device->getVkHandle(), &smci, nullptr, &this->handle) != VK_SUCCESS)
    {
        knM_logFatal("Can't create shader module");
        return false;
    }

    return true;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola

    