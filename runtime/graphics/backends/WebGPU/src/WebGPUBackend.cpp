#include "backends/WebGPU/WebGPUBackend.hpp"

#include "backends/WebGPU/WebGPUDebug.hpp"
#include "backends/WebGPU/WebGPULimits.hpp"
#include "backends/WebGPU/WebGPUCommandBuffer.hpp"
#include "backends/WebGPU/WebGPUCommandEncoder.hpp"
#include "backends/WebGPU/WebGPUMipmapGeneratorShader.hpp"

#include <unordered_set>
#include <memory>

#include <webgpu/webgpu.hpp>

using namespace wgpu;

namespace SLGL::Graphics {
    WebGPU::Backend::Backend(const std::vector<QueueFamily>& requestedQueues) {
        // Initialize WebGPU
        InstanceDescriptor instanceDesc = Default;
        instance = createInstance(instanceDesc);

        // Request an adapter
        RequestAdapterOptions adapterOpts = Default;
        adapterOpts.featureLevel = FeatureLevel::Core;
        adapterOpts.powerPreference = PowerPreference::HighPerformance;
        adapter = instance.requestAdapter(adapterOpts);

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
        deviceDesc.deviceLostCallbackInfo.userdata1 = &isDeviceLossIntentional;
        deviceDesc.deviceLostCallbackInfo.callback = [](WGPUDevice const *, WGPUDeviceLostReason reason,
                                                        WGPUStringView message, void* userdata, void*) {
            bool isDeviceLossIntentional = *static_cast<bool*>(userdata);
            if (isDeviceLossIntentional) return;

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
        wgpu::Queue queue = ctx.device.getQueue();
        queueFamilies.emplace_back(QueueFamily {
            .capability = QueueFamily::Capability::Graphics | QueueFamily::Capability::Compute | QueueFamily::Capability::Transfer,
            .count = 1,
        }, std::vector<Queue*>{
            queue,
        });

        mipmapGenerator = std::make_unique<Graphics::MipmapGenerator>(this, mipmapGeneratorShader);
    }

    WebGPU::Backend::~Backend() {
        for(auto& [_, queues] : queueFamilies) {
            for(auto queue : queues) {
                ((wgpu::Queue)(WGPUQueue)queue).release();
            }
        }

        adapter.release();
        instance.release();

        isDeviceLossIntentional = true;
    }

    const std::vector<std::pair<QueueFamily, std::vector<Queue*>>>& WebGPU::Backend::GetQueueFamilies() {
        return queueFamilies;
    }

    Graphics::CommandEncoder::Ref WebGPU::Backend::CreateCommandEncoder(const std::string &label) {
        CommandEncoderDescriptor desc = Default;
        desc.label = wgpu::StringView(label);
        return std::make_shared<WebGPU::CommandEncoder>(ctx.device.createCommandEncoder(desc), label);
    }

    void WebGPU::Backend::SubmitCommands(Graphics::Queue* _queue, Graphics::CommandBuffer::Ref commandBuffer) {
        ((wgpu::Queue)(WGPUQueue)_queue).submit(dynamic_cast<CommandBuffer*>(commandBuffer.get())->GetHandle());
    }

    void WebGPU::Backend::SubmitCommands(Graphics::Queue* _queue, const std::vector<Graphics::CommandBuffer::Ref>& commandBuffers) {
        std::vector<wgpu::CommandBuffer> wgpuCommandBuffers;
        wgpuCommandBuffers.reserve(commandBuffers.size());
        for(const auto& commandBuffer : commandBuffers) {
            wgpuCommandBuffers.push_back(dynamic_cast<CommandBuffer*>(commandBuffer.get())->GetHandle());
        }
        ((wgpu::Queue)(WGPUQueue)_queue).submit(wgpuCommandBuffers.size(), wgpuCommandBuffers.data());
    }

    void WebGPU::Backend::Tick() {
    #if defined(WEBGPU_BACKEND_DAWN)
        ctx.device.tick();
    #elif defined(WEBGPU_BACKEND_WGPU)
        ctx.device.poll(false, nullptr);
    #endif
    }

    Graphics::MipmapGenerator* WebGPU::Backend::GetMipmapGenerator() const {
        return mipmapGenerator.get();
    }

    Instance WebGPU::Backend::getInstance() const {
        return instance;
    }

    Adapter WebGPU::Backend::getAdapter() const {
        return adapter;
    }

    wgpu::Device WebGPU::Backend::getDevice() const {
        return ctx.device;
    }
}