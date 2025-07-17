#include "component_registry.hpp"

#include <iostream>

namespace SHAME::Engine::ECS::Components {

    ComponentRegistry gSharedComponentRegistry;

    void ComponentRegistry::RegisterComponentType(const std::string& name, ComponentFactory factory) {
        if (registry.find(name) != registry.end()) {
            throw std::runtime_error("[ERROR] Component already registered: " + name);
        }
        registry[name] = factory;
        std::cout << "[INFO] Registered: " << name << std::endl;
    }

    Component* ComponentRegistry::CreateComponentByName(const std::string& name) {
        auto it = registry.find(name);
        if (it == registry.end()) {
            throw std::runtime_error("[ERROR] Component not registered: " + name);
        }
        return it->second();
    }

    const std::unordered_map<std::string, ComponentFactory>& ComponentRegistry::GetAll() const {
        return registry;
    }
}