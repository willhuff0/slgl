#pragma once

#include "Buffer.hpp"
#include "RenderPass.hpp"
#include "RenderEncoder.hpp"
#include "CommandBuffer.hpp"

#include <functional>
#include <string>

namespace SLGL::Graphics {
    class CommandEncoder {
    public:
        typedef std::shared_ptr<CommandEncoder> Ref;

        virtual ~CommandEncoder() = default;

        virtual void SetLabel(const std::string& newLabel) = 0;

        [[nodiscard]] virtual const std::string& GetLabel() const = 0;

        virtual void ClearBuffer(Buffer::Ref buffer, uint64_t offset, uint64_t size) = 0;
        virtual void ClearBuffer(Buffer::Ref buffer, uint64_t offset) = 0;
        virtual void ClearBuffer(Buffer::Ref buffer) = 0;

        virtual void CopyBuffer(Buffer::Ref from, uint64_t fromOffset, Buffer::Ref to, uint64_t toOffset, uint64_t size) = 0;
        virtual void CopyBuffer(Buffer::Ref from, Buffer::Ref to, uint64_t size) = 0;
        virtual void CopyBuffer(Buffer::Ref from, Buffer::Ref to) = 0;

        virtual void EncodeRenderPass(const RenderPass& renderPass, const std::function<void(RenderEncoder*)>& func) = 0;

        virtual CommandBuffer::Ref Finish(const std::string& label) = 0;
    };
}