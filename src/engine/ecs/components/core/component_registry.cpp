#include "component_registry.hpp"

namespace SHAME::Engine::ECS::Components {

std::unordered_map<std::string, ComponentFactory> ComponentRegistry::registry;

}