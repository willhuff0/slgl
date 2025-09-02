#include "GFX/WebGPUCommandEncoder.hpp"

#include "GFX/WebGPUCommandBuffer.hpp"
#include "GFX/WebGPUTexture.hpp"
#include "GFX/WebGPURenderEncoder.hpp"

#include <webgpu/webgpu.hpp>

#include <memory>

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

    static wgpu::LoadOp convertLoadOp(Graphics::CommandEncoder::RenderPass::LoadOp loadOp) {
        switch (loadOp) {
            case Graphics::CommandEncoder::RenderPass::LoadOp::Clear: return wgpu::LoadOp::Clear;
            case Graphics::CommandEncoder::RenderPass::LoadOp::Load: return wgpu::LoadOp::Load;
            default: throw std::runtime_error("Unexpected load op");
        }
    }

    static wgpu::StoreOp convertStoreOp(Graphics::CommandEncoder::RenderPass::StoreOp storeOp) {
        switch (storeOp) {
            case Graphics::CommandEncoder::RenderPass::StoreOp::Store: return wgpu::StoreOp::Store;
            case Graphics::CommandEncoder::RenderPass::StoreOp::Discard: return wgpu::StoreOp::Discard;
            default: throw std::runtime_error("Unexpected store op");
        }
    }

    static wgpu::Color convertColor(glm::vec4 color) {
        return { color.r, color.g, color.b, color.a };
    }

    static wgpu::RenderPassDepthStencilAttachment convertDepthStencilAttachment(const Graphics::CommandEncoder::RenderPass::DepthStencilAttachment& depthStencilAttachment) {
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

    void WebGPU::CommandEncoder::EncodeRenderPass(const Graphics::CommandEncoder::RenderPass &renderPass, const std::function<void(Graphics::RenderEncoder*)>& func) {
        wgpu::RenderPassDescriptor desc;
        desc.label = wgpu::StringView(renderPass.label);
        desc.colorAttachmentCount = renderPass.colorAttachments.size();
        std::vector<wgpu::RenderPassColorAttachment> colorAttachments;
        colorAttachments.reserve(desc.colorAttachmentCount);
        for(const auto& colorAttachment : renderPass.colorAttachments) {
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