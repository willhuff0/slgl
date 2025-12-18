#pragma once

#include "Texture.hpp"

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <utility>

namespace SLGL::Graphics {
    class RenderBundleEncoder;

    class RenderBundle {
    public:
        typedef std::shared_ptr<RenderBundle> Ref;

        class Builder {
        public:
            class Impl {
            public:
                virtual ~Impl() = default;
                virtual Ref BuildAndEncode(Builder& builder, const std::function<void(Graphics::RenderBundleEncoder*)>& func) = 0;
            };

            explicit Builder(Impl* impl);

            Builder& SetLabel(const std::string& newLabel);
            Builder& SetColorFormats(std::vector<std::pair<Texture::Format, Texture::ColorSpace>> newColorFormats);
            Builder& SetDepthStencilFormat(std::optional<Texture::Format> newDepthStencilFormat);
            Builder& SetSampleCount(uint32_t newSampleCount);
            Builder& SetDepthReadOnly(bool newDepthReadOnly);
            Builder& SetStencilReadOnly(bool newStencilReadOnly);

            [[nodiscard]] const std::string& getLabel() const;
            [[nodiscard]] const std::vector<std::pair<Texture::Format, Texture::ColorSpace>>& getColorFormats() const;
            [[nodiscard]] const std::optional<Texture::Format>& getDepthStencilFormat() const;
            [[nodiscard]] uint32_t getSampleCount() const;
            [[nodiscard]] bool isDepthReadOnly() const;
            [[nodiscard]] bool isStencilReadOnly() const;

            Ref BuildAndEncode(const std::function<void(Graphics::RenderBundleEncoder*)>& func);

        private:
            Impl* impl;
            std::string label = "SLGL RenderBundle";
            std::vector<std::pair<Texture::Format, Texture::ColorSpace>> colorFormats;
            std::optional<Texture::Format> depthStencilFormat;
            uint32_t sampleCount = 1;
            bool depthReadOnly = false;
            bool stencilReadOnly = false;
        };

        virtual ~RenderBundle() = default;

        virtual void SetLabel(const std::string& newLabel) = 0;

        [[nodiscard]] virtual const std::string& GetLabel() const = 0;
    };
}