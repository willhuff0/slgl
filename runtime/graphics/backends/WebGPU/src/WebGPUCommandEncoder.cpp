#include "backends/WebGPU/WebGPUCommandEncoder.hpp"

#include "backends/WebGPU/WebGPUCommandBuffer.hpp"
#include "backends/WebGPU/WebGPUTexture.hpp"
#include "backends/WebGPU/WebGPUBuffer.hpp"
#include "backends/WebGPU/WebGPURenderEncoder.hpp"

#include <webgpu/webgpu.hpp>

#include <memory>
#include <algorithm>

namespace SLGL::Graphics {
    WebGPU::CommandEncoder::CommandEncoder(wgpu::CommandEncoder commandEncoder, std::string label) : commandEncoder(commandEncoder), label(std::move(label)) { }
    WebGPU::CommandEncoder::~CommandEncoder() {
        commandEncoder.release();
    }

    wgpu::CommandEncoder WebGPU::CommandEncoder::GetHandle() const {
        return commandEncoder;
    }

    const std::string& WebGPU::CommandEncoder::GetLabel() const {
        return label;
    }

    void WebGPU::CommandEncoder::SetLabel(const std::string &newLabel) {
        label = newLabel;
        commandEncoder.setLabel(wgpu::StringView(label));
    }

    static wgpu::LoadOp convertLoadOp(Graphics::RenderPass::LoadOp loadOp) {
        switch (loadOp) {
            case Graphics::RenderPass::LoadOp::None: return wgpu::LoadOp::Undefined;
            case Graphics::RenderPass::LoadOp::Clear: return wgpu::LoadOp::Clear;
            case Graphics::RenderPass::LoadOp::Load: return wgpu::LoadOp::Load;
            default: throw std::runtime_error("Unexpected load op");
        }
    }

    static wgpu::StoreOp convertStoreOp(Graphics::RenderPass::StoreOp storeOp) {
        switch (storeOp) {
            case Graphics::RenderPass::StoreOp::None: return wgpu::StoreOp::Undefined;
            case Graphics::RenderPass::StoreOp::Store: return wgpu::StoreOp::Store;
            case Graphics::RenderPass::StoreOp::Discard: return wgpu::StoreOp::Discard;
            default: throw std::runtime_error("Unexpected store op");
        }
    }

    static wgpu::Color convertColor(glm::vec4 color) {
        return { color.r, color.g, color.b, color.a };
    }

    static wgpu::RenderPassDepthStencilAttachment convertDepthStencilAttachment(const Graphics::RenderPass::DepthStencilAttachment& depthStencilAttachment) {
        wgpu::RenderPassDepthStencilAttachment result = wgpu::Default;

        result.depthLoadOp = convertLoadOp(depthStencilAttachment.depthLoadOp);
        result.depthStoreOp = convertStoreOp(depthStencilAttachment.depthStoreOp);
        result.depthClearValue = depthStencilAttachment.depthClearValue;
        result.depthReadOnly = depthStencilAttachment.depthReadOnly;

        result.stencilLoadOp = convertLoadOp(depthStencilAttachment.stencilLoadOp);
        result.stencilStoreOp = convertStoreOp(depthStencilAttachment.stencilStoreOp);
        result.stencilClearValue = depthStencilAttachment.stencilClearValue;
        result.stencilReadOnly = depthStencilAttachment.stencilReadOnly;

        result.view = dynamic_cast<WebGPU::Texture::View*>(depthStencilAttachment.textureView.get())->GetHandle();

        return result;
    }

    void WebGPU::CommandEncoder::ClearBuffer(Buffer::Ref buffer, uint64_t offset, uint64_t size) {
        commandEncoder.clearBuffer(dynamic_cast<WebGPU::Buffer*>(buffer.get())->GetHandle(), offset, size);
    }
    void WebGPU::CommandEncoder::ClearBuffer(Buffer::Ref buffer, uint64_t offset) {
        ClearBuffer(buffer, offset, buffer->GetSize() - offset);
    }
    void WebGPU::CommandEncoder::ClearBuffer(Buffer::Ref buffer) {
        ClearBuffer(buffer, 0, buffer->GetSize());
    }

    void WebGPU::CommandEncoder::CopyBuffer(Buffer::Ref from, uint64_t fromOffset, Buffer::Ref to, uint64_t toOffset, uint64_t size) {
        commandEncoder.copyBufferToBuffer(dynamic_cast<WebGPU::Buffer*>(from.get())->GetHandle(), fromOffset, dynamic_cast<WebGPU::Buffer*>(to.get())->GetHandle(), toOffset, size);
    }
    void WebGPU::CommandEncoder::CopyBuffer(Buffer::Ref from, Buffer::Ref to, uint64_t size) {
        CopyBuffer(from, 0, to, 0, size);
    }
    void WebGPU::CommandEncoder::CopyBuffer(Buffer::Ref from, Buffer::Ref to) {
        CopyBuffer(from, 0, to, 0, std::min<uint64_t>(from->GetSize(), to->GetSize()));
    }

    void WebGPU::CommandEncoder::EncodeRenderPass(const Graphics::RenderPass& renderPass, const std::function<void(Graphics::RenderEncoder*)>& func) {
        wgpu::RenderPassDescriptor desc = wgpu::Default;
        desc.label = wgpu::StringView(renderPass.label);
        desc.colorAttachmentCount = renderPass.colorAttachments.size();
        std::vector<wgpu::RenderPassColorAttachment> colorAttachments;
        colorAttachments.reserve(desc.colorAttachmentCount);
        for (const auto& colorAttachment : renderPass.colorAttachments) {
            wgpu::RenderPassColorAttachment result = wgpu::Default;
            result.view = dynamic_cast<WebGPU::Texture::View*>(colorAttachment.textureView.get())->GetHandle();
            if (colorAttachment.resolveTextureView != nullptr) result.resolveTarget = dynamic_cast<WebGPU::Texture::View*>(colorAttachment.resolveTextureView.get())->GetHandle();
            result.loadOp = convertLoadOp(colorAttachment.loadOp);
            result.storeOp = convertStoreOp(colorAttachment.storeOp);
            result.clearValue = convertColor(colorAttachment.clearColor);
            colorAttachments.push_back(result);
        }
        desc.colorAttachments = colorAttachments.data();
        wgpu::RenderPassDepthStencilAttachment depthStencilAttachment;
        if (renderPass.depthStencilAttachment.has_value()) {
            depthStencilAttachment = convertDepthStencilAttachment(renderPass.depthStencilAttachment.value());
            desc.depthStencilAttachment = &depthStencilAttachment;
        }

        wgpu::RenderPassEncoder renderPassEncoder = commandEncoder.beginRenderPass(desc);
        WebGPU::RenderEncoder renderEncoder(renderPassEncoder);
        func(&renderEncoder);
        renderPassEncoder.end();
        renderPassEncoder.release();
    }

    Graphics::CommandBuffer::Ref WebGPU::CommandEncoder::Finish(const std::string& _label) {
        return std::make_shared<CommandBuffer>(commandEncoder.finish(), _label);
    }
}