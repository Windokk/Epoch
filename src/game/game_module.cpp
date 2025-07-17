#include "engine/ecs/components/core/component_registry.hpp"
#include "engine/ecs/components/core/dll_component_registration.hpp"

using namespace SHAME::Engine::ECS::Components;

extern "C" __declspec(dllexport) void InitializeComponentRegistry(SHAME::Engine::ECS::Components::ComponentRegistry* ptr) {
    SetComponentRegistry(ptr);
}

extern "C" __declspec(dllexport)
void RegisterGameComponents() {
    for (auto& cb : GetComponentRegistrars()) {
        cb(GetComponentRegistry());
    }
}