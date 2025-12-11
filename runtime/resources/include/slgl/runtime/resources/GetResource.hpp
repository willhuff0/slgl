#pragma once

#include "Resource.hpp"
#include "ResourceLoader.hpp"

#include <string>

template<std::derived_from<Resource> T, std::size_t N>
class GetResource {
public:
    consteval explicit GetResource(const char (&path)[N]);

    [[nodiscard]] consteval std::string_view GetPath() const;

    T& operator()();

private:
    class ResourceContainer {
    public:
        ResourceContainer();

        T resource;
    };

    std::array<char, N> path;
};

template<std::derived_from<Resource> T, std::size_t N>
consteval GetResource<T, N>::GetResource(const char (& path)[N]) {
    for (std::size_t i = 0; i < N; i++) {
        path[i] = path[i];
    }
}

template<std::derived_from<Resource> T, std::size_t N>
consteval std::string_view GetResource<T, N>::GetPath() const {
    return std::string_view(path.data, N - 1);
}

template<std::derived_from<Resource> T, std::size_t N>
T& GetResource<T, N>::operator()() {
    static ResourceContainer container;
    return container.resource;
}

template<std::derived_from <Resource> T, std::size_t N>
GetResource<T, N>::ResourceContainer::ResourceContainer() {

}
