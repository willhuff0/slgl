#pragma once

#include "Resource.hpp"

#include <string>
#include <unordered_map>
#include <memory>

template<bool isGeneratorConnected>
class ResourceLoader {
public:
    explicit ResourceLoader(std::string basePath);

    Resource* GetResource(const std::string& path);

private:
    std::unordered_map<std::string, std::unique_ptr<Resource>> resources;
    std::string basePath;
};

template<bool isGeneratorConnected>
Resource* ResourceLoader<isGeneratorConnected>::GetResource(std::string path) {
    return nullptr;
}

template<bool isGeneratorConnected>
ResourceLoader<isGeneratorConnected>::ResourceLoader(const std::string& basePath) {

}

