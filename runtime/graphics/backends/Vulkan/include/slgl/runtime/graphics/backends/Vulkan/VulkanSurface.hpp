#pragma once

#include <slgl/runtime/graphics/Surface.hpp>

#include <slgl/runtime/platform/Window.hpp>

#include <vulkan/vulkan.hpp>

#include <cstdint>

namespace SLGL::Graphics::Vulkan {
    class Surface : public Graphics::Surface, public Platform::WindowSizeSubject::Observer {
    public:
        Surface(Platform::Window::Ref window, vk::SurfaceKHR handle, bool vsync, const vk::Adapter& adapter, vk::Device device);
        ~Surface() override;

        [[nodiscard]] vk::SurfaceKHR GetHandle() const;
        [[nodiscard]] uint32_t GetFormat() const override;
        [[nodiscard]] int GetWidth() const override;
        [[nodiscard]] int GetHeight() const override;
        [[nodiscard]] int IsVSyncEnabled() const override;

        [[nodiscard]] Graphics::Texture::View::Ref GetCurrentTextureView() override;
        void Present() override;

        void SetVSyncEnabled(bool vsync) override;

        void onWindowSizeChanged(const int& width, const int& height) override;

    private:
        Platform::Window::Ref window;

        vk::SurfaceKHR handle;
        vk::SurfaceCapabilities capabilities;

        vk::SurfaceConfiguration config = wgpu::Default;
        vk::TextureViewDescriptor surfaceTextureViewDesc = wgpu::Default;
    };
}