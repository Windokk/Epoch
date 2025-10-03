#include "object.hpp"

#include "engine/debugging/debugger.hpp"

namespace Epoch::Engine::ECS::Objects{
    
    Object::Object()
    {

    }

    Object::~Object()
    {

    }

    std::shared_ptr<Object> Object::GetChild(int index)
    {
        return ObjectIDManager::GetInstance().GetObjectFromID(children[index]);
    }

    std::shared_ptr<Object> Object::GetChild(ObjectID ObjectID)
    {
        if (GetID() == ObjectID)
            return shared_from_this();

        for (auto& child : children)
        {
            if (child == ObjectID)
                return ObjectIDManager::GetInstance().GetObjectFromID(child);
        }

        return nullptr;
    }

    void Object::AddChild(std::shared_ptr<Object>o)
    {
        children.push_back(o->GetID());
        o->SetParent(id);
    }
}