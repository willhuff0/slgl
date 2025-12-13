#pragma once

#include "Resource.hpp"

#include <memory>

class ResourceGenerator {
public:
    static ResourceGenerator* GetInstance();

    std::unique_ptr<Resource> GenerateResource(const std::string& path);
};