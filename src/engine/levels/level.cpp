#include "level.hpp"
#include <iostream>

#include "engine/ecs/objectID.hpp"
#include "engine/ecs/objects/actors/actor.hpp"

#include "engine/ecs/components/rendering/light_component.hpp"
#include "engine/ecs/components/rendering/model_component.hpp"

namespace SHAME::Engine::Levels{

    Level::Level(std::string name)
    {
        this->name = name;
    }
    
    void Level::Save(const std::string &filepath)
    {

    }

    void Level::Clear()
    {
        for(auto& actor : rootActors){
            RemoveActor(actor->GetID(), true);
        }
    }

    void Level::AddActor(ECS::Objects::Actor* a)
    {
        rootActors.push_back(a);
        a->SetLevel(this);
    }

    void Level::RemoveActor(ECS::ObjectID id, bool recursive)
    {
        GetActor(id, recursive)->Destroy();
        for (int i = 0; i < rootActors.size(); i++)
        {
            if (rootActors[i]->GetID() == id){
                rootActors.erase(rootActors.begin()+i);
            }
        }
    }

    ECS::Objects::Actor* Level::GetActor(ECS::ObjectID id, bool recursive)
    {
        for (auto& actorPtr : rootActors)
        {
            if (actorPtr->GetID() == id)
                return actorPtr;

            if(recursive){
                std::vector<ECS::ObjectID> children = actorPtr->GetChildrenID(true);

                for(auto& _id : children){
                    ECS::Objects::Actor* child = dynamic_cast<ECS::Objects::Actor*>(ECS::ObjectIDManager::GetObjectFromID(_id));
                    if(_id == id && child){
                        return child;
                    }
                }
            }
        }

        return nullptr;
    }

    std::vector<ECS::ObjectID> Level::GetActorsID(bool recursive)
    {
        std::vector<ECS::ObjectID> actorIDs;

        for (auto& actorPtr : rootActors)
        {
            actorIDs.push_back(actorPtr->GetID());

            if(recursive){
                std::vector<ECS::ObjectID> children = actorPtr->GetChildrenID(true);
                actorIDs.insert(actorIDs.end(), children.begin(), children.end());
            }
        }

        return actorIDs;
    }

    const std::string &Level::GetName() const
    {
        return name;
    }

    void Level::SetName(const std::string &name)
    {
        this->name = name;
    }
}