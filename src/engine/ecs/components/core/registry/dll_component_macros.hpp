#pragma once

#include "engine/ecs/components/core/component.hpp"
#include "component_registry.hpp"
#include "dll_component_registration.hpp"

using namespace SHAME::Engine::ECS;

#define BEGIN_COMPONENT(className, baseClass)                                                   \
    class className : public baseClass {                                                        \
public:                                                                                         \
        className(Objects::Actor* parent, uint32_t local_id);

#define END_COMPONENT(className)                                                                \
    };                                                                                          \
    inline Components::Component* Create_##className() { return new className(nullptr, 0); }    \

#define REGISTER_COMPONENT(className)                        \
namespace {                                                 \
    struct AutoRegister_##className {                       \
        AutoRegister_##className() {                        \
            SHAME::Engine::ECS::Components::AddComponentRegistrar( \
                [](SHAME::Engine::ECS::Components::ComponentRegistry& reg) { \
                    reg.RegisterComponentType(#className, Create_##className); \
                }                                           \
            );                                              \
        }                                                   \
    };                                                      \
    static AutoRegister_##className autoRegister_##className; \
}