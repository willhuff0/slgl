#pragma once

#include "Texture.hpp"

#include <slgl/common/Math.hpp>

#include <string>
#include <vector>
#include <optional>
#include <cstdint>

namespace SLGL::Graphics {
    struct RenderPass {
        enum class LoadOp : uint8_t {
            None,
            Load,
            Clear,
        };
        enum class StoreOp : uint8_t {
            None,
            Store,
            Discard,
        };

        struct ColorAttachment {
            Texture::View::Ref textureView = nullptr;
            Texture::View::Ref resolveTextureView = nullptr;
            LoadOp loadOp = LoadOp::Clear;
            StoreOp storeOp = StoreOp::Store;
            glm::vec4 clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        };
        struct DepthStencilAttachment {
            LoadOp depthLoadOp = LoadOp::Clear;
            StoreOp depthStoreOp = StoreOp::Store;
            float depthClearValue = 1.0f;
            bool depthReadOnly = false;

            LoadOp stencilLoadOp = LoadOp::None;
            StoreOp stencilStoreOp = StoreOp::None;
            uint32_t stencilClearValue = 0;
            bool stencilReadOnly = false;

            Texture::View::Ref textureView = nullptr;
        };

        std::string label;
        std::vector<ColorAttachment> colorAttachments;
        std::optional<DepthStencilAttachment> depthStencilAttachment;
    };
}