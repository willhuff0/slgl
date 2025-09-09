#pragma once

#include <slgl/runtime/graphics/CommandBuffer.hpp>

#include <webgpu/webgpu.hpp>

#include <string>

namespace SLGL::Graphics::WebGPU {
    class CommandBuffer : public Graphics::CommandBuffer {
    public:
        CommandBuffer(wgpu::CommandBuffer commandBuffer, std::string label);
        ~CommandBuffer() override;

        void SetLabel(const std::string &newLabel) override;

        [[nodiscard]] wgpu::CommandBuffer GetHandle() const;
        [[nodiscard]] const std::string& GetLabel() const override;

    private:
        wgpu::CommandBuffer commandBuffer;
        std::string label;
    };
}