#pragma once 

#include <functional>
#include <cstdint>

namespace kola {
namespace utils {

template <typename T>
struct enum_hasher
{
    size_t operator()(const T& t) const
    {
        return std::hash<int64_t>()(static_cast<int64_t>(t));
    }
};

} // namespace utils
} // namespace kola