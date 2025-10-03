#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>

#include "engine/debugging/debugger.hpp"

namespace Epoch::Engine::ECS::Components {

    class Component;
    using ComponentFactory = Component* (*)();

    class ComponentRegistry {
        public:
            void RegisterComponentType(const std::string& name, ComponentFactory factory);
            Component* CreateComponentByName(const std::string& name);
            const std::unordered_map<std::string, ComponentFactory>& GetAll() const;

            ComponentRegistry() = default;
            ComponentRegistry(const ComponentRegistry&) = delete;
            ComponentRegistry& operator=(const ComponentRegistry&) = delete;

        private:
            std::unordered_map<std::string, ComponentFactory> registry;
    };

    extern ComponentRegistry gSharedComponentRegistry;

#if defined(BUILD_ENGINE)

    // Used by the EXE/engine
    inline ComponentRegistry& GetComponentRegistry() {
        return gSharedComponentRegistry;
    }

#else

    // Used by the DLL
    inline ComponentRegistry* gSharedComponentRegistryPtr = nullptr;

    inline void SetComponentRegistry(ComponentRegistry* ptr) {
        gSharedComponentRegistryPtr = ptr;
    }

    inline ComponentRegistry& GetComponentRegistry() {
        if (!gSharedComponentRegistryPtr)
            DEBUG_FATAL("ComponentRegistry pointer not initialized!");
        return *gSharedComponentRegistryPtr;
    }

#endif

}