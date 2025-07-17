#pragma once

#include "component.hpp"
#include "component_registry.hpp"
using namespace SHAME::Engine::ECS;

#define BEGIN_COMPONENT(className, baseClass)        \
    class className : public baseClass {                      \
    public:                                                   \
        className(Objects::Actor* parent, uint32_t local_id);

#define END_COMPONENT(className)                                \
    };                                                          \
    inline Components::Component* Create_##className() { return new className(nullptr, 0); }
    