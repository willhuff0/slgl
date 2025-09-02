#pragma once

#include "Surface.hpp"

#include <slgl/common/utils/ObserverPattern.hpp>

namespace SLGL::Graphics {
    OBSERVER_PATTERN(SurfaceSize, int, int)

    class SurfaceFactory : public SurfaceSizeSubject {
    public:
        virtual ~SurfaceFactory() = default;

        virtual Surface* GetSurface(void* instance) = 0;
        virtual int GetWidth() = 0;
        virtual int GetHeight() = 0;
    };
}