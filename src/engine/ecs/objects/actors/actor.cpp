#include "actor.hpp"


namespace SHAME::Engine::ECS::Objects{
    
    using namespace Components;

    Actor::Actor(std::string name)
    {
        SetName(name);

        transform = new Transform(this, this->components.size());
        components.push_back(transform);
    }

    void Actor::AddChild(Object *o)
    {
        Object::AddChild(o);
        if (Actor* actorChild = dynamic_cast<Actor*>(o)) {
            actorChild->SetLevel(this->level);
        }
    }

    void Actor::SetLevel(Levels::Level* lvl)
    {
        this->level = lvl;
    }
}