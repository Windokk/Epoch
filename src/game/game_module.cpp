#include "engine/ecs/components/core/component_registry.hpp"
#include "character.hpp"

extern "C" __declspec(dllexport) void InitializeComponentRegistry(SHAME::Engine::ECS::Components::ComponentRegistry* ptr) {
    SHAME::Engine::ECS::Components::SetComponentRegistry(ptr);
}

extern "C" __declspec(dllexport)
void RegisterGameComponents() {
    using namespace SHAME::Engine::ECS::Components;
    GetComponentRegistry().RegisterComponentType("Character", Create_Character);
}