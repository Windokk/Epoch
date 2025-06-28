#include "actor.hpp"


namespace SHAME::Engine::ECS::Objects{
    
    using namespace Components;

    Actor::Actor(std::string name)
    {
        SetName(name);

        Transform* tr = new Transform(this, this->components.size(), glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(1,1,1));
        components.push_back(tr);
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