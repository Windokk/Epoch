#pragma once

#include "component.hpp"
#include "component_registry.hpp"

#define BEGIN_COMPONENT(className, baseClass)        \
    class className : public baseClass {                      \
    public:                                                   \
        className(SHAME::Engine::ECS::Objects::Actor* parent, uint32_t local_id);

#define END_COMPONENT(className)                                \
    };                                                          \
    SHAME::Engine::ECS::Components::Component* Create_##className() { return new className(nullptr, 0); } \
    struct AutoRegister_##className {                           \
        AutoRegister_##className() {                            \
            SHAME::Engine::ECS::Components::ComponentRegistry::RegisterComponentType(#className, Create_##className); \
        }                                                       \
    } autoRegister_##className;