#pragma once

#include <slgl/runtime/graphics/Surface.hpp>

#include <webgpu/webgpu.hpp>

#include <cstdint>

namespace SLGL::Graphics::WebGPU {
    class Surface : public Graphics::Surface {
    public:
        explicit Surface(wgpu::Surface handle);
        ~Surface() override;

        [[nodiscard]] wgpu::Surface GetHandle() const;
        [[nodiscard]] uint32_t GetFormat() const override;
        [[nodiscard]] int GetWidth() const override;
        [[nodiscard]] int GetHeight() const override;

        void SetConfiguration(wgpu::SurfaceConfiguration newConfig);
        void EditConfiguration(const std::function<void(wgpu::SurfaceConfiguration&)>& func);

        [[nodiscard]] Graphics::Texture::View::Ref GetCurrentTextureView() override;
        void Present() override;

    private:
        wgpu::Surface handle;
        wgpu::SurfaceConfiguration config;
        bool configured = false;

        wgpu::TextureViewDescriptor surfaceTextureViewDesc = wgpu::Default;
    };
}