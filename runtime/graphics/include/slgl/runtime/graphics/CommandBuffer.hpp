#pragma once

#include "Queue.hpp"

#include <memory>
#include <string>

namespace SLGL::Graphics {
    class CommandBuffer {
    public:
        typedef std::shared_ptr<CommandBuffer> Ref;

        virtual ~CommandBuffer() = default;

        virtual void SetLabel(const std::string& newLabel) = 0;

        [[nodiscard]] virtual const std::string& GetLabel() const = 0;
    };
}