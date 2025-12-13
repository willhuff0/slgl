#pragma once

#include "Resource.hpp"
#include "ResourceGenerator.hpp"

#include <string>
#include <unordered_map>
#include <memory>
#include <utility>
#include <stdexcept>

template<bool isGeneratorAvailable>
class ResourceLoader {
public:
    explicit ResourceLoader(std::string basePath);

    Resource* GetResource(const std::string& path);

private:
    std::string basePath;
    std::unordered_map<std::string, std::unique_ptr<Resource>> resources;
};

template<bool isGeneratorConnected>
ResourceLoader<isGeneratorConnected>::ResourceLoader(std::string basePath) : basePath(std::move(basePath)) { }

template<bool isGeneratorConnected>
Resource* ResourceLoader<isGeneratorConnected>::GetResource(const std::string& path) {
    auto iter = resources.find(path);
    if (iter == resources.end()) {
        if constexpr (isGeneratorConnected) {
            auto resource = ResourceGenerator::GetInstance()->GenerateResource(basePath + path);
            auto result = resources.emplace(path, resource);
            return result.first->second.get();
        }
        throw std::runtime_error("Resource at '" + path + "' not found and the generator is not available.");
    }
    return iter->second.get();
}

