#pragma once

#include <slgl/runtime/graphics/Buffer.hpp>

#include "backends/WebGPU/WebGPUContext.hpp"

#include <webgpu/webgpu.hpp>

#include <string>
#include <cstdint>

namespace SLGL::Graphics::WebGPU {
    class Buffer : public Graphics::Buffer {
    public:
        class Builder : public Graphics::Buffer::Builder::Impl {
        public:
            explicit Builder(Context& ctx);

            Ref Build(Graphics::Buffer::Builder &builder) override;

        private:
            Context& ctx;
        };

        Buffer(wgpu::Buffer buffer, std::string label);
        ~Buffer() override;

        void SetLabel(const std::string &newLabel) override;

        wgpu::Buffer GetHandle();
        const std::string &GetLabel() override;
        uint64_t GetSize() override;

        void Write(Graphics::Queue* queue, void *src, uint64_t offset, uint64_t size) override;
        void Write(Graphics::Queue* queue, void *src) override;

    private:
        wgpu::Buffer buffer;
        std::string label;
    };
}
