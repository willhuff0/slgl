#pragma once

#include <slgl/runtime/graphics/CommandEncoder.hpp>

#include <webgpu/webgpu.hpp>

#include <string>

namespace SLGL::Graphics::WebGPU {
    class CommandEncoder : public Graphics::CommandEncoder {
    public:
        CommandEncoder(wgpu::CommandEncoder commandEncoder, std::string label);
        ~CommandEncoder() override;

        void SetLabel(const std::string& newLabel) override;

        [[nodiscard]] wgpu::CommandEncoder GetHandle() const;
        [[nodiscard]] const std::string& GetLabel() const override;

        void ClearBuffer(Buffer::Ref buffer, uint64_t offset, uint64_t size) override;
        void ClearBuffer(Buffer::Ref buffer, uint64_t offset) override;
        void ClearBuffer(Buffer::Ref buffer) override;

        void CopyBuffer(Buffer::Ref from, uint64_t fromOffset, Buffer::Ref to, uint64_t toOffset, uint64_t size) override;
        void CopyBuffer(Buffer::Ref from, Buffer::Ref to, uint64_t size) override;
        void CopyBuffer(Buffer::Ref from, Buffer::Ref to) override;

        void EncodeRenderPass(const Graphics::RenderPass& renderPass, const std::function<void(Graphics::RenderEncoder*)>& func) override;

        Graphics::CommandBuffer::Ref Finish(const std::string& label) override;

    private:
        wgpu::CommandEncoder commandEncoder;
        std::string label;
    };
}