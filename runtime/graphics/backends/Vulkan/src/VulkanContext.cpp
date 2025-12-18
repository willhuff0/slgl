#include "backends/Vulkan/VulkanContext.hpp"

namespace SLGL::Graphics {
    Vulkan::Context::~Context() {
        device.destroy();
    }
}