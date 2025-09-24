#include "engine/ecs/components/core/registry/component_registry.hpp"
#include "engine/ecs/components/core/registry/dll_component_registration.hpp"

#include "engine/inputs/input_manager.hpp"

using namespace EPOCH::Engine;
using namespace EPOCH::Engine::ECS::Components;
using namespace EPOCH::Engine::Input;

extern "C" __declspec(dllexport) void InitializeSingletons(Debugging::Debugger* debugger, Debugging::Level minDebugLevel, ComponentRegistry* compReg, InputManager* inputMan) {
    SetDebugger(debugger);
    Debugging::GetDebugger().SetMinimumLevel(minDebugLevel);
    SetComponentRegistry(compReg);
    SetInputManager(inputMan);
}

extern "C" __declspec(dllexport)
void RegisterGameComponents() {
    for (auto& cb : GetComponentRegistrars()) {
        cb(GetComponentRegistry());
    }
}