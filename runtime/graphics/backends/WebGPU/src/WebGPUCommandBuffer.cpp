#include "backends/WebGPU/WebGPUCommandBuffer.hpp"

#include <webgpu/webgpu.hpp>

namespace SLGL::Graphics {
    WebGPU::CommandBuffer::CommandBuffer(wgpu::CommandBuffer commandBuffer, std::string label)
        : commandBuffer(commandBuffer), label(std::move(label)) {}
    WebGPU::CommandBuffer::~CommandBuffer() {
        commandBuffer.release();
    }

    wgpu::CommandBuffer WebGPU::CommandBuffer::GetHandle() const {
        return commandBuffer;
    }

    const std::string& WebGPU::CommandBuffer::GetLabel() const {
        return label;
    }

    void WebGPU::CommandBuffer::SetLabel(const std::string& newLabel) {
        label = newLabel;
        commandBuffer.setLabel(wgpu::StringView(label));
    }
}