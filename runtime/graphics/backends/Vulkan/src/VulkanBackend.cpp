#include "backends/Vulkan/VulkanBackend.hpp"

namespace SLGL::Graphics {
    Vulkan::Backend::Backend(const std::vector<SLGL::Graphics::QueueFamily>& requestedQueueFamilies) {
        vk::ApplicationInfo appInfo {
            .pApplicationName = "SLGL Game",
            .applicationVersion = 1,
            .pEngineName = "SLGL",
            .engineVersion = 1,
            .apiVersion = VK_API_VERSION_1_2,
        };

        instance = vk::createInstance(vk::InstanceCreateInfo {
            .pApplicationInfo = &appInfo,
            .enabledExtensionCount =
        });
    }
}