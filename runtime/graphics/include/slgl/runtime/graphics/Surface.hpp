#pragma once

#include "Texture.hpp"

#include <slgl/common/utils/ObserverPattern.hpp>

#include <memory>
#include <cstdint>

namespace SLGL::Graphics {
    OBSERVER_PATTERN(SurfaceSize, int, int)

    class Surface : public SurfaceSizeSubject {
    public:
        typedef std::shared_ptr<Surface> Ref;

        virtual ~Surface() = default;

        [[nodiscard]] virtual uint32_t GetFormat() const = 0;
        [[nodiscard]] virtual int GetWidth() const = 0;
        [[nodiscard]] virtual int GetHeight() const = 0;
        [[nodiscard]] virtual int IsVSyncEnabled() const = 0;

        [[nodiscard]] virtual Texture::View::Ref GetCurrentTextureView() = 0;
        virtual void Present() = 0;

        virtual void SetVSyncEnabled(bool vsync) = 0;
    };
}