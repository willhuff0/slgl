#include <slgl/runtime/game/ecs/systems/MeshRendererSystem.hpp>

#include <slgl/runtime/game/Provider.hpp>
#include <slgl/runtime/game/ecs/components/NodeComponent.hpp>
#include <slgl/runtime/game/ecs/components/MeshComponent.hpp>

MeshRendererSystem::MeshRendererSystem() {
    key = GetDecs()->RegisterSystem<NodeComponent, MeshComponent>(
        [](const NodeComponent& nodeComp, const MeshComponent& meshComp) {
            auto node = nodeComp.GetNode();
            if (!node->IsEnabled()) return;

            if (node)
        });
}

MeshRendererSystem::~MeshRendererSystem() {
    GetDecs()->UnregisterSystem(key);
}
