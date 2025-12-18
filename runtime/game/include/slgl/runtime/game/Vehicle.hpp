#pragma once

#include <slgl/runtime/graphics/Graphics.hpp>

#include <string>
#include <cstdint>
#include <unordered_map>

using namespace SLGL::Graphics;

class PartType {
    std::string name;
    float maxHealth;

    Buffer::Ref positionBuffer;
    Buffer::Ref normalBuffer;
    Buffer::Ref indexBuffer;
    uint32_t indexCount;
};

class PartInstanceArray {

};

class Vehicle {
public:
    void Draw(RenderEncoder* renderEncoder);

private:
    std::unordered_map<PartType*, PartInstanceArray> parts;
    Buffer::Ref indirectBuffer;
};
