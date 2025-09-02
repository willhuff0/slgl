#include "GFX/WebGPUSurface.hpp"

#include "GFX/WebGPUTexture.hpp"

#include <memory>

namespace SLGL::Graphics {
    WebGPU::Surface::Surface(wgpu::Surface handle) {
        this->handle = handle;

        surfaceTextureViewDesc.label = wgpu::StringView("SLGL Surface Texture View");
        surfaceTextureViewDesc.dimension = wgpu::TextureViewDimension::_2D;
        surfaceTextureViewDesc.baseMipLevel = 0;
        surfaceTextureViewDesc.mipLevelCount = 1;
        surfaceTextureViewDesc.baseArrayLayer = 0;
        surfaceTextureViewDesc.arrayLayerCount = 1;
        surfaceTextureViewDesc.aspect = wgpu::TextureAspect::All;
        surfaceTextureViewDesc.format = wgpu::TextureFormat::Undefined;
    }

    WebGPU::Surface::~Surface() {
        if (configured) handle.unconfigure();
        handle.release();
    }

    wgpu::Surface WebGPU::Surface::GetHandle() const {
        return handle;
    }

    uint32_t WebGPU::Surface::GetFormat() const {
        return config.format;
    }

    int WebGPU::Surface::GetWidth() const {
        return config.width;
    }

    int WebGPU::Surface::GetHeight() const {
        return config.height;
    }

    void WebGPU::Surface::SetConfiguration(wgpu::SurfaceConfiguration newConfig) {
        config = newConfig;
        handle.configure(config);
        configured = true;
    }

    void WebGPU::Surface::EditConfiguration(const std::function<void(wgpu::SurfaceConfiguration&)> &func) {
        if (!configured) config = wgpu::Default;

        func(config);
        handle.configure(config);
        configured = true;
    }

    Graphics::Texture::View::Ref WebGPU::Surface::GetCurrentTextureView() {
        wgpu::SurfaceTexture surfaceTexture;
        handle.getCurrentTexture(&surfaceTexture);
        if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal) {
            std::cerr << "Failed to get current surface texture!" << std::endl;
            return nullptr;
        }
        wgpu::Texture texture = surfaceTexture.texture;

        surfaceTextureViewDesc.format = texture.getFormat();
        wgpu::TextureView textureView = texture.createView(surfaceTextureViewDesc);

        Texture::View::Ref result = std::make_shared<Texture::View>(textureView, "SLGL Surface Texture View");

        texture.release();
        return result;
    }

    void WebGPU::Surface::Present() {
        handle.present();
    }
}