#pragma once

#include <slgl/runtime/platform/IWindow.hpp>

#include "Texture.hpp"

#include <cstdint>

namespace SLGL::Graphics {
    class Surface {
    public:
        virtual ~Surface() = default;

        [[nodiscard]] virtual uint32_t GetFormat() const = 0;
        [[nodiscard]] virtual int GetWidth() const = 0;
        [[nodiscard]] virtual int GetHeight() const = 0;

        [[nodiscard]] virtual Texture::View::Ref GetCurrentTextureView() = 0;
        virtual void Present() = 0;
    };
}