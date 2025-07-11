#include "object.hpp"

namespace SHAME::Engine::ECS::Objects{
    
    Object::Object()
    {
    }
    
    std::shared_ptr<Object> Object::Create()
    {
        std::shared_ptr<Object> obj(new Object());
        obj->id = ECS::ObjectIDManager::GenerateNewID();
        ECS::ObjectIDManager::AssignID(obj->id, obj);
        return obj;
    }
    Object::~Object()
    {

    }

    std::shared_ptr<Object> Object::GetChild(int index)
    {
        return ObjectIDManager::GetObjectFromID(children[index]);
    }

    std::shared_ptr<Object> Object::GetChild(ObjectID ObjectID)
    {
        if (GetID() == ObjectID)
            return shared_from_this();

        for (auto& child : children)
        {
            if (child == ObjectID)
                return ObjectIDManager::GetObjectFromID(child);
        }

        return nullptr;
    }

    void Object::AddChild(std::shared_ptr<Object>o)
    {
        children.push_back(o->GetID());
        o->SetParent(id);
    }
}