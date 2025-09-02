#pragma once

#include "RenderEncoder.hpp"
#include "CommandBuffer.hpp"

#include <slgl/common/Math.hpp>

#include <vector>
#include <functional>
#include <optional>
#include <cstdint>

namespace SLGL::Graphics {
    class CommandEncoder {
    public:
        typedef std::shared_ptr<CommandEncoder> Ref;

        struct RenderPass {
            enum class LoadOp : uint8_t {
                Load,
                Clear,
            };
            enum class StoreOp : uint8_t {
                Store,
                Discard,
            };

            struct ColorAttachment {
                Texture::View::Ref textureView = nullptr;
                Texture::View::Ref resolveTextureView = nullptr;
                LoadOp loadOp = LoadOp::Clear;
                StoreOp storeOp = StoreOp::Store;
                glm::vec4 clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            };
            struct DepthStencilAttachment {
                LoadOp depthLoadOp = LoadOp::Clear;
                StoreOp depthStoreOp = StoreOp::Store;
                float depthClearValue = 0.0f;
                bool depthReadOnly = false;

                LoadOp stencilLoadOp = LoadOp::Clear;
                StoreOp stencilStoreOp = StoreOp::Store;
                uint32_t stencilClearValue = 0;
                bool stencilReadOnly = false;

                Texture::View::Ref textureView = nullptr;
            };

            std::string label;
            std::vector<ColorAttachment> colorAttachments;
            std::optional<DepthStencilAttachment> depthStencilAttachment;
        };

        virtual ~CommandEncoder() = default;

        virtual void SetLabel(const std::string& newLabel) = 0;

        [[nodiscard]] virtual const std::string& GetLabel() const = 0;

        virtual void EncodeRenderPass(const RenderPass& renderPass, const std::function<void(RenderEncoder*)>& func) = 0;

        virtual CommandBuffer::Ref Finish(const std::string& label) = 0;
    };
}