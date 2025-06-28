#include "component.hpp"

namespace SHAME::Engine::ECS::Components {

    Component::Component(Objects::Actor *parent, uint32_t local_id)
    {
        this->parent = parent;
        this->local_id = local_id;
    }

    Component::~Component()
    {
    }

    void Component::Destroy() {
        
    }

}