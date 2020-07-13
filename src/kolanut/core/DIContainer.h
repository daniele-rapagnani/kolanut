#pragma once

#include <memory>
#include <functional>

namespace kola {
namespace di {

template <typename T>
using CreateCallback = std::function<std::shared_ptr<T>(void)>;

template <typename T>
void registerType(CreateCallback<T>);

template <typename T>
class Container
{
public:
    template <typename U>
    friend void registerType(CreateCallback<U>);

public:
    static std::shared_ptr<T> get()
    {
        if (!Container<T>::instance)
        {
            Container<T>::instance = Container<T>::cb();
        }

        return Container<T>::instance;
    }

private:
    static std::shared_ptr<T> instance;
    static CreateCallback<T> cb;
};

template <typename T>
std::shared_ptr<T> Container<T>::instance = nullptr;

template <typename T>
CreateCallback<T> Container<T>::cb = nullptr;

template <typename T>
void registerType(CreateCallback<T> createCb)
{
    Container<T>::cb = createCb;
}

template <typename T>
std::shared_ptr<T> get()
{
    return Container<T>::get();
}

template <typename T>
std::shared_ptr<T> create()
{
    return Container<T>::get();
}

} // namespace di
} // namespace kola