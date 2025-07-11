#include "actor.hpp"


namespace SHAME::Engine::ECS::Objects{
    
    using namespace Components;

    Actor::Actor(std::string name)
    {
        SetName(name);

        transform = new Transform(this, this->components.size());
        components.push_back(transform);
    }

    void Actor::AddChild(std::shared_ptr<Object> o)
    {
        Object::AddChild(o);
        if (std::shared_ptr<Actor> actorChild = std::dynamic_pointer_cast<Actor>(o)) {
            actorChild->SetLevel(this->level);
        }
    }

    void Actor::SetLevel(Levels::Level* lvl)
    {
        this->level = lvl;
    }
}