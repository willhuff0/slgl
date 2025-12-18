#pragma once

#include <vulkan/vulkan.hpp>

namespace SLGL::Graphics::Vulkan {
    class Context {
    public:
        ~Context();

        vk::Device device;
    };
}