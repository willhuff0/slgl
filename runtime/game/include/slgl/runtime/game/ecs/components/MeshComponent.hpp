#pragma once

#include <slgl/runtime/graphics/Graphics.hpp>

using namespace SLGL::Graphics;

struct MeshComponent {
public:


private:
    Buffer::Ref indexBuffer;
    Buffer::Ref positionBuffer;
    Buffer::Ref normalBuffer;
    Buffer::Ref texCoordBuffer;
    BindSet::Ref bindSet;
};