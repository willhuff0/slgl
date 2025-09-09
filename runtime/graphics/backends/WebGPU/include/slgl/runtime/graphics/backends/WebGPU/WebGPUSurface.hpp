#pragma once

#include <slgl/runtime/graphics/Surface.hpp>

#include <slgl/runtime/platform/Window.hpp>

#include <webgpu/webgpu.hpp>

#include <cstdint>

namespace SLGL::Graphics::WebGPU {
    class Surface : public Graphics::Surface, public Platform::WindowSizeSubject::Observer {
    public:
        Surface(Platform::Window::Ref window, wgpu::Surface handle, bool vsync,
                const wgpu::Adapter& adapter, wgpu::Device device);
        ~Surface() override;

        [[nodiscard]] wgpu::Surface GetHandle() const;
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

        wgpu::Surface handle;
        wgpu::SurfaceCapabilities capabilities;

        wgpu::SurfaceConfiguration config = wgpu::Default;
        wgpu::TextureViewDescriptor surfaceTextureViewDesc = wgpu::Default;
    };
}