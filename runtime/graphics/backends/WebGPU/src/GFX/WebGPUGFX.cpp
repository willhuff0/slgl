#include <GFX/WebGPUGFX.hpp>

#include "WebGPUDebug.hpp"
#include "WebGPULimits.hpp"
#include "GFX/WebGPUCommandBuffer.hpp"
#include "GFX/WebGPUCommandEncoder.hpp"
#include "WebGPUMipmapGeneratorShader.hpp"

#include <unordered_set>
#include <memory>

#include <webgpu/webgpu.hpp>

using namespace wgpu;

namespace SLGL::Graphics {
    static PresentMode getBestPresentMode(const SurfaceCapabilities &capabilities, const bool vsync = true) {
        std::unordered_set<WGPUPresentMode> presentModes;
        for (size_t i = 0; i < capabilities.presentModeCount; ++i) {
            presentModes.insert(capabilities.presentModes[i]);
        }

        if (vsync) {
            if (presentModes.find(PresentMode::Mailbox) != presentModes.end()) return PresentMode::Mailbox;
            if (presentModes.find(PresentMode::FifoRelaxed) != presentModes.end()) return PresentMode::FifoRelaxed;
            if (presentModes.find(PresentMode::Fifo) != presentModes.end()) return PresentMode::Fifo;
            std::cerr << "VSync was requested, but the adapter does not support it! Falling back to VSync off."
                      << std::endl;
        }

        if (presentModes.find(PresentMode::Immediate) != presentModes.end()) return PresentMode::Immediate;
        throw std::runtime_error("Adapter does not support VSync off.");
    }

    static TextureFormat getBestSurfaceFormat(const SurfaceCapabilities& capabilities) {
        std::unordered_set<WGPUTextureFormat> surfaceFormats;
        for (size_t i = 0; i < capabilities.formatCount; ++i) {
            surfaceFormats.insert(capabilities.formats[i]);
        }

        // Prioritize SRGB formats
        if (surfaceFormats.find(TextureFormat::RGBA8UnormSrgb) != surfaceFormats.end()) return TextureFormat::RGBA8UnormSrgb;
        if (surfaceFormats.find(TextureFormat::BGRA8UnormSrgb) != surfaceFormats.end()) return TextureFormat::BGRA8UnormSrgb;

        // Fallback to adapter preferred format
        return capabilities.formats[0];
    }

    void WebGPU::GFX::Initialize(Graphics::SurfaceFactory* surfaceFactory, bool vsync) {
        // Initialize WebGPU
        InstanceDescriptor instanceDesc = Default;
        instance = createInstance(instanceDesc);

        // Get the surface
        ctx.surface = (Surface*)surfaceFactory->GetSurface(instance);
        surfaceSizeSubject = surfaceFactory;
        surfaceSizeSubject->AddSurfaceSizeObserver(this);

        // Request an adapter
        RequestAdapterOptions adapterOpts = Default;
        adapterOpts.compatibleSurface = ctx.surface->GetHandle();
        adapterOpts.featureLevel = FeatureLevel::Core;
        adapterOpts.powerPreference = PowerPreference::HighPerformance;
        Adapter adapter = instance.requestAdapter(adapterOpts);

    #if !defined(NDEBUG)
        DebugPrintAdapter(adapter);
    #endif

        // Get required limits
        Limits requiredLimits = GetRequiredLimits(adapter);

        // Request a device
        DeviceDescriptor deviceDesc = Default;
        deviceDesc.label = wgpu::StringView("SLGL Default Device");
        deviceDesc.requiredFeatureCount = 0;
        deviceDesc.requiredLimits = &requiredLimits;

        // Setup default queue
        deviceDesc.defaultQueue.label = wgpu::StringView("SLGL Default Queue");

        // Setup error callbacks
        deviceDesc.deviceLostCallbackInfo.mode = wgpu::CallbackMode::AllowSpontaneous;
        deviceDesc.deviceLostCallbackInfo.callback = [](WGPUDevice const *, WGPUDeviceLostReason reason,
                                                        WGPUStringView message, void *, void *) {
            std::cout << std::endl << "==============================" << std::endl
                      << "[Graphics Backend (WebGPU)] Device lost: " << std::endl << std::endl;
            if (message.length > 0 && message.data) std::cout << message.data << std::endl;
            std::cout << "==============================" << std::endl;
        };

        deviceDesc.uncapturedErrorCallbackInfo.callback = [](WGPUDevice const *, WGPUErrorType type,
                                                             WGPUStringView message, void *, void *) {
            std::cout << std::endl << "==============================" << std::endl
                      << "[Graphics Backend (WebGPU)] Uncaptured device error: " << type << std::endl << std::endl;
            if (message.length > 0 && message.data) std::cout << message.data << std::endl;
            std::cout << "==============================" << std::endl;
        };

        ctx.device = adapter.requestDevice(deviceDesc);

        // Get limits
        Limits actualLimits;
        ctx.device.getLimits(&actualLimits);

        // Get the queue
        queue = ctx.device.getQueue();

        // Configure the surface
        SurfaceCapabilities surfaceCapabilities;
        ctx.surface->GetHandle().getCapabilities(adapter, &surfaceCapabilities);

        PresentMode presentMode = getBestPresentMode(surfaceCapabilities, vsync);
    #if !defined(NDEBUG)
        DebugPrintPresentMode(presentMode);
        std::cout << std::endl;
    #endif

        ctx.surface->EditConfiguration([&](SurfaceConfiguration& config) {
            config.usage = TextureUsage::RenderAttachment;
            config.width = surfaceFactory->GetWidth();
            config.height = surfaceFactory->GetHeight();
            config.format = getBestSurfaceFormat(surfaceCapabilities);
            config.viewFormatCount = 0;
            config.viewFormats = nullptr;
            config.device = ctx.device;
            config.alphaMode = CompositeAlphaMode::Auto;
            config.presentMode = presentMode;
        });

        // Adapter no longer needed
        adapter.release();

        mipmapGenerator = std::make_unique<Graphics::MipmapGenerator>(this, mipmapGeneratorShader);
    }

    WebGPU::GFX::~GFX() {
        if (surfaceSizeSubject != nullptr) surfaceSizeSubject->RemoveSurfaceSizeObserver(this);
    }

    Graphics::Surface *WebGPU::GFX::GetSurface() {
        return ctx.surface;
    }

    Graphics::CommandEncoder::Ref WebGPU::GFX::CreateCommandEncoder(const std::string &label) {
        CommandEncoderDescriptor desc = Default;
        desc.label = wgpu::StringView(label);
        return std::make_shared<WebGPU::CommandEncoder>(ctx.device.createCommandEncoder(desc), label);
    }

    void WebGPU::GFX::SubmitCommands(Graphics::Queue* _queue, Graphics::CommandBuffer::Ref commandBuffer) {
        ((wgpu::Queue)(WGPUQueue)_queue).submit(dynamic_cast<CommandBuffer*>(commandBuffer.get())->GetHandle());
    }

    void WebGPU::GFX::SubmitCommands(Graphics::Queue* _queue, const std::vector<Graphics::CommandBuffer::Ref>& commandBuffers) {
        std::vector<wgpu::CommandBuffer> wgpuCommandBuffers;
        wgpuCommandBuffers.reserve(commandBuffers.size());
        for(const auto& commandBuffer : commandBuffers) {
            wgpuCommandBuffers.push_back(dynamic_cast<CommandBuffer*>(commandBuffer.get())->GetHandle());
        }
        ((wgpu::Queue)(WGPUQueue)_queue).submit(wgpuCommandBuffers.size(), wgpuCommandBuffers.data());
    }

    void WebGPU::GFX::Tick() {
    #if defined(WEBGPU_BACKEND_DAWN)
        ctx.device.tick();
    #elif defined(WEBGPU_BACKEND_WGPU)
        ctx.device.poll(false, nullptr);
    #endif
    }

    Graphics::MipmapGenerator* WebGPU::GFX::GetMipmapGenerator() const {
        return mipmapGenerator.get();
    }

    void WebGPU::GFX::onSurfaceSizeChanged(const int& width, const int& height) {
        ctx.surface->EditConfiguration([&](SurfaceConfiguration& config) {
            config.width = width;
            config.height = height;
        });
    }
}