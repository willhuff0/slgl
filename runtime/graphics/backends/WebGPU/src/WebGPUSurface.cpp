#include "backends/WebGPU/WebGPUSurface.hpp"

#include "backends/WebGPU/WebGPUTexture.hpp"
#include "backends/WebGPU/WebGPUDebug.hpp"

#include <webgpu/webgpu.hpp>

#include <memory>
#include <unordered_set>

namespace SLGL::Graphics {
    static wgpu::TextureFormat getBestSurfaceFormat(const wgpu::SurfaceCapabilities& capabilities) {
        std::unordered_set<WGPUTextureFormat> surfaceFormats;
        for (size_t i = 0; i < capabilities.formatCount; ++i) {
            surfaceFormats.insert(capabilities.formats[i]);
        }

        // Prioritize SRGB formats
        if (surfaceFormats.contains(wgpu::TextureFormat::RGBA8UnormSrgb)) return wgpu::TextureFormat::RGBA8UnormSrgb;
        if (surfaceFormats.contains(wgpu::TextureFormat::BGRA8UnormSrgb)) return wgpu::TextureFormat::BGRA8UnormSrgb;

        // Fallback to adapter preferred format
        return capabilities.formats[0];
    }

    static wgpu::PresentMode getBestPresentMode(const wgpu::SurfaceCapabilities& capabilities, const bool vsync = true) {
        std::unordered_set<WGPUPresentMode> presentModes;
        for (size_t i = 0; i < capabilities.presentModeCount; ++i) {
            presentModes.insert(capabilities.presentModes[i]);
        }

        if (vsync) {
            if (presentModes.contains(wgpu::PresentMode::Mailbox)) return wgpu::PresentMode::Mailbox;
            if (presentModes.contains(wgpu::PresentMode::FifoRelaxed)) return wgpu::PresentMode::FifoRelaxed;
            if (presentModes.contains(wgpu::PresentMode::Fifo)) return wgpu::PresentMode::Fifo;
            std::cerr << "VSync was requested, but the adapter does not support it! Falling back to VSync off." << std::endl;
        }

        if (presentModes.contains(wgpu::PresentMode::Immediate)) return wgpu::PresentMode::Immediate;
        throw std::runtime_error("Adapter does not support VSync off.");
    }

    WebGPU::Surface::Surface(Platform::Window::Ref _window, wgpu::Surface _handle, bool vsync,
                             const wgpu::Adapter& adapter, wgpu::Device device)
                             : window(std::move(_window)), handle(_handle) {
        handle.getCapabilities(adapter, &capabilities);

        config.usage = wgpu::TextureUsage::RenderAttachment;
        config.width = window->GetHeight();
        config.height = window->GetHeight();
        config.format = getBestSurfaceFormat(capabilities);
        config.viewFormatCount = 0;
        config.viewFormats = nullptr;
        config.device = device;
        config.alphaMode = wgpu::CompositeAlphaMode::Auto;
        config.presentMode = getBestPresentMode(capabilities, vsync);
#if !defined(NDEBUG)
        DebugPrintPresentMode(config.presentMode);
        std::cout << std::endl;
#endif
        handle.configure(config);

        surfaceTextureViewDesc.label = wgpu::StringView("SLGL Surface Texture View");
        surfaceTextureViewDesc.dimension = wgpu::TextureViewDimension::_2D;
        surfaceTextureViewDesc.baseMipLevel = 0;
        surfaceTextureViewDesc.mipLevelCount = 1;
        surfaceTextureViewDesc.baseArrayLayer = 0;
        surfaceTextureViewDesc.arrayLayerCount = 1;
        surfaceTextureViewDesc.aspect = wgpu::TextureAspect::All;
        surfaceTextureViewDesc.format = wgpu::TextureFormat::Undefined;

        window->AddWindowSizeObserver(this);
    }

    WebGPU::Surface::~Surface() {
        window->RemoveWindowSizeObserver(this);

        handle.unconfigure();
        handle.release();
    }

    wgpu::Surface WebGPU::Surface::GetHandle() const {
        return handle;
    }

    uint32_t WebGPU::Surface::GetFormat() const {
        return config.format;
    }

    int WebGPU::Surface::GetWidth() const {
        return static_cast<int>(config.width);
    }

    int WebGPU::Surface::GetHeight() const {
        return static_cast<int>(config.height);
    }

    int WebGPU::Surface::IsVSyncEnabled() const {
        switch (config.presentMode) {
            case wgpu::PresentMode::Mailbox:
            case wgpu::PresentMode::FifoRelaxed:
            case wgpu::PresentMode::Fifo:
                return true;
            default:
                return false;
        }
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

    void WebGPU::Surface::SetVSyncEnabled(bool vsync) {
        config.presentMode = getBestPresentMode(capabilities, vsync);
        handle.configure(config);
    }

    void WebGPU::Surface::onWindowSizeChanged(const int& width, const int& height) {
        config.width = width;
        config.height = height;
        handle.configure(config);
        notifySurfaceSizeChanged(width, height);
    }
}