#pragma once

#include "../WebGPUContext.hpp"
#include "WebGPUGFX.hpp"

#include <slgl/runtime/graphics/Texture.hpp>

#include <webgpu/webgpu.hpp>

#include <memory>
#include <string>
#include <cstdint>

namespace SLGL::Graphics::WebGPU {
    class Texture : public Graphics::Texture, public std::enable_shared_from_this<Texture> {
    public:
        class View : public Graphics::Texture::View {
        public:
            class Builder : public Graphics::Texture::View::Builder::Impl {
            public:
                Ref Build(Graphics::Texture::View::Builder &builder) override;
            };

            View(wgpu::TextureView textureView, std::string label);
            ~View() override;

            void SetLabel(const std::string &newLabel) override;

            wgpu::TextureView GetHandle();
            const std::string& GetLabel() override;

        private:
            wgpu::TextureView textureView;
            std::string label;
        };

        class Builder : public Graphics::Texture::Builder::Impl {
        public:
            explicit Builder(Context& ctx);

            Ref Build(Graphics::Texture::Builder &builder) override;

        private:
            Context& ctx;
        };

        Texture(wgpu::Texture texture, std::string label);
        ~Texture() override;

        void SetLabel(const std::string &newLabel) override;

        wgpu::Texture GetHandle();
        const std::string &GetLabel() override;
        glm::ivec2 GetSize() override;
        Dimension GetDimension() override;
        Format GetFormat() override;
        ColorSpace GetColorSpace() override;
        uint32_t GetMipLevels() override;
        uint32_t GetSamples() override;

        uint32_t GetSurfaceFormat() override;

        void Write(Graphics::Queue *queue, void *src, size_t size, int mipLevel, glm::ivec2 offset, glm::ivec2 extent) override;
        void Write(Graphics::Queue *queue, void *src, size_t size, int mipLevel) override;
        void Write(Graphics::Queue *queue, void *src, size_t size) override;

        Graphics::Texture::View::Builder CreateView() override;

        static Texture::Ref FromData(GFX* gfx, Graphics::Queue *queue, const std::string& label, const Data::Image& image, int mipLevels = 1);

    private:
        wgpu::Texture texture;
        std::string label;
    };
}
