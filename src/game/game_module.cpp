#include "engine/ecs/components/core/registry/component_registry.hpp"
#include "engine/ecs/components/core/registry/dll_component_registration.hpp"

#include "engine/inputs/input_manager.hpp"

using namespace SHAME::Engine::ECS::Components;
using namespace SHAME::Engine::Input;

extern "C" __declspec(dllexport) void InitializeSingletons(ComponentRegistry* compReg, InputManager* inputMan) {
    SetComponentRegistry(compReg);
    SetInputManager(inputMan);
}

extern "C" __declspec(dllexport)
void RegisterGameComponents() {
    for (auto& cb : GetComponentRegistrars()) {
        cb(GetComponentRegistry());
    }
}