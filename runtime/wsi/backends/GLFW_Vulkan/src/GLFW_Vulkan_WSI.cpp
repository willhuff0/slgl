#include "backends/GLFW_Vulkan_WSI.hpp"

#include <slgl/runtime/graphics/backends/Vulkan/VulkanBackend.hpp>
#include <slgl/runtime/graphics/backends/Vulkan/VulkanSurface.hpp>
#include <slgl/runtime/platform/backends/GLFW/GLFWWindow.hpp>

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

#if defined(_GLFW_X11)
#  define GLFW_EXPOSE_NATIVE_X11
#endif
#if defined(_GLFW_WAYLAND)
#  define GLFW_EXPOSE_NATIVE_WAYLAND
#endif
#if defined(_GLFW_COCOA)
#  define GLFW_EXPOSE_NATIVE_COCOA
#endif
#if defined(_GLFW_WIN32)
#  define GLFW_EXPOSE_NATIVE_WIN32
#endif

#if defined(GLFW_EXPOSE_NATIVE_COCOA)
#  include <Foundation/Foundation.h>
#  include <QuartzCore/CAMetalLayer.h>
#endif

#include <GLFW/glfw3native.h>
#include <vulkan/vulkan_win32.h>

#include <memory>
#include <cstdint>
#include <stdexcept>

static vk::SurfaceKHR createWindowSurface(vk::Instance instance, GLFWwindow* window) {
    switch (glfwGetPlatform()) {
#if defined(GLFW_EXPOSE_NATIVE_X11)
        case GLFW_PLATFORM_X11: {

        }
#endif

#if defined(GLFW_EXPOSE_NATIVE_WAYLAND)
        case GLFW_PLATFORM_WAYLAND: {

        }
#endif

#if defined(GLFW_EXPOSE_NATIVE_COCOA)
        case GLFW_PLATFORM_COCOA: {

        }
#endif

#if defined(GLFW_EXPOSE_NATIVE_WIN32)
        case GLFW_PLATFORM_WIN32: {
            VkSurfaceKHR surface;

            HWND hwnd = glfwGetWin32Window(window);
            HINSTANCE hinstance = GetModuleHandle(nullptr);

            VkWin32SurfaceCreateInfoKHR createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            createInfo.hwnd = hwnd;
            createInfo.hinstance = hinstance;

            if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create surface!");
            }

            return surface;
        }
#endif

        default:
            return nullptr;
    }
}

namespace SLGL::WSI::GLFW_Vulkan {
    Graphics::Surface::Ref CreateSurface(Graphics::Backend* baseGfx, Platform::Window::Ref baseWindow, bool vsync) {
        auto gfx = dynamic_cast<Graphics::Vulkan::Backend*>(baseGfx);
        auto window = std::dynamic_pointer_cast<Platform::GLFW::Window>(baseWindow);

        auto handle = createWindowSurface(gfx->getInstance(), window->getHandle());
        if (handle == nullptr) {
            throw std::runtime_error("Failed to create native glfw surface!");
        }

        return std::make_shared<Graphics::Vulkan::Surface>(window, handle, vsync, gfx->getDevice());
    }

    std::vector<std::string> GetRequiredExtensions(Platform::Backend* backend) {
        uint32_t count;
        const char** extensions = glfwGetRequiredInstanceExtensions(&count);

        std::vector<std::string> result;
        result.reserve(count);
        for(uint32_t i = 0; i < count; i++) {
            result.emplace_back(extensions[i]);
        }

        return result;
    }
}
