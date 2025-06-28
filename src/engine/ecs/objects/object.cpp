#include "object.hpp"

namespace SHAME::Engine::ECS::Objects{
    
    Object::Object()
    {
        id = ECS::ObjectIDManager::GenerateNewID();
        ECS::ObjectIDManager::AssignID(id, this);
    }
    Object::~Object()
    {

    }

    Object *Object::GetChild(int index)
    {
        return ObjectIDManager::GetObjectFromID(children[index]);
    }

    Object* Object::GetChild(ObjectID ObjectID)
    {
        if (GetID() == ObjectID)
            return this;

        for (auto& child : children)
        {
            if (child == ObjectID)
                return ObjectIDManager::GetObjectFromID(child);
        }

        return nullptr;
    }

    void Object::AddChild(Object *o)
    {
        children.push_back(o->GetID());
        o->SetParent(id);
    }
}