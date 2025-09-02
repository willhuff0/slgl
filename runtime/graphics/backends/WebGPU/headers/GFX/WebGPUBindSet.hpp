#pragma once

#include "../WebGPUContext.hpp"

#include <slgl/runtime/graphics/BindSet.hpp>

#include <webgpu/webgpu.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <cstdint>

namespace SLGL::Graphics::WebGPU {
    class BindSet : public Graphics::BindSet {
    public:
        class Layout : public Graphics::BindSet::Layout, public std::enable_shared_from_this<Layout> {
        public:
            class Builder : public Graphics::BindSet::Layout::Builder::Impl {
            public:
                explicit Builder(Context& ctx);

                Ref Build(Graphics::BindSet::Layout::Builder &builder) override;

            private:
                Context& ctx;
            };

            Layout(Context& ctx, wgpu::BindGroupLayout bindGroupLayout, std::string label,
                   std::unordered_map<uint32_t, Entry> entries);
            ~Layout() override;

            void SetLabel(const std::string &newLabel) override;

            wgpu::BindGroupLayout GetHandle();
            const std::string &GetLabel() override;
            const std::unordered_map<uint32_t, Entry>& GetEntries() override;

            Graphics::BindSet::Builder Instantiate() override;

        private:
            Context& ctx;
            wgpu::BindGroupLayout bindGroupLayout;
            std::string label;
            std::unordered_map<uint32_t, Entry> entries;
        };

        class Builder : public Graphics::BindSet::Builder::Impl {
        public:
            explicit Builder(Context& ctx);

            Ref Build(Graphics::BindSet::Builder &builder) override;

        private:
            Context& ctx;
        };

        BindSet(wgpu::BindGroup bindGroup, std::string label, Layout::Ref layout);
        ~BindSet() override;

        void SetLabel(const std::string &newLabel) override;

        wgpu::BindGroup GetHandle();
        const std::string &GetLabel() override;
        Layout::Ref GetLayout() override;

    private:
        wgpu::BindGroup bindGroup;
        std::string label;
        Layout::Ref layout;
    };
}
