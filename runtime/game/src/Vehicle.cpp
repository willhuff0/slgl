#include <slgl/runtime/game/Vehicle.hpp>

#include <slgl/runtime/game/Provider.hpp>

void Vehicle::Draw(RenderEncoder* renderEncoder) {
    renderEncoder->MultiDrawInstancesIndexedIndirect(indirectBuffer, parts.size());
}
