#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

#if defined(BUILD_ENGINE)
    #define ECS_API __declspec(dllexport)
#else
    #define ECS_API __declspec(dllimport)
#endif

namespace SHAME::Engine::ECS::Components{

    class Component;

    using ComponentFactory = Component* (*)();

    class ECS_API ComponentRegistry {
        public:

            static void RegisterComponentType(const std::string& name, ComponentFactory factory) {
                if (registry.find(name) != registry.end()) {
                    throw std::runtime_error("[ERROR] [ENGINE/ECS/COMPONENTS/COMPONENT_REGISTRY] : Component already registered: " + name);
                }
                registry[name] = factory;
                std::cout << "[INFO] [ENGINE/ECS/COMPONENTS/COMPONENT_REGISTRY] : Registered: " << name << std::endl;
                std::cout << "[INFO] [ENGINE/ECS/COMPONENTS/COMPONENT_REGISTRY] : Registry size : " << registry.size() << std::endl;
                std::cout << "[DEBUG] Registry address: " << &registry << std::endl;
            }

            static Component* CreateComponentByName(const std::string& name) {
                std::cout << "[INFO] [ENGINE/ECS/COMPONENTS/COMPONENT_REGISTRY] : Registry size : " << registry.size() << std::endl;
                std::cout << "[DEBUG] Registry address: " << &registry << std::endl;
                auto it = registry.find(name);
                if (it == registry.end()) {
                    throw std::runtime_error("[ERROR] [ENGINE/ECS/COMPONENTS/COMPONENT_REGISTRY] : Component not registered: " + name);
                }
                return it->second();
            }

        private:
            static std::unordered_map<std::string, ComponentFactory> registry;

            // Disallow external construction
            ComponentRegistry() = default;
            ComponentRegistry(const ComponentRegistry&) = delete;
            ComponentRegistry& operator=(const ComponentRegistry&) = delete;
    };
}

