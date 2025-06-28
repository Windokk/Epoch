#pragma once

#include <vector>
#include <memory>

#include "engine/ecs/objectID.hpp"


namespace SHAME::Engine::ECS::Objects{
    
    class Object{
        public:
        Object();
        virtual ~Object();

        Object* GetChild(int index);
        Object* GetChild(ObjectID id);
        std::vector<ObjectID> GetChildrenID(bool recursive = false){ 
            std::vector<ObjectID> ids;

            ids.insert(ids.end(), children.begin(), children.end());
            
            if (recursive) {
                for (const auto& childID : children) {
                    Object* child = GetChild(childID);
                    if (child) {
                        std::vector<ObjectID> subChildren = child->GetChildrenID(true);
                        ids.insert(ids.end(), subChildren.begin(), subChildren.end());
                    }
                }
            }
            
            return ids;
        }
        int GetChildrenCount() { return children.size(); }

        virtual void AddChild(Object* o);

        void DeleteChildRef(ObjectID child){
            for(int i = 0; i < children.size(); i++){
                if(children[i].GetAsInt() == child.GetAsInt()){
                    children.erase(children.begin()+i);
                }
            }
        }

        Object* GetParent() { return ObjectIDManager::GetObjectFromID(parent); }
        void SetParent(ObjectID parentID) { this->parent = parentID; }

        ObjectID GetID() { return id; }

        virtual void Destroy(){
            if(parent.GetAsInt() != -1){
                ObjectIDManager::GetObjectFromID(parent)->DeleteChildRef(id);
            }
            for(auto& child : children)
            { 
                ObjectIDManager::GetObjectFromID(child)->Destroy(); 
            }
            children.clear();
            ObjectIDManager::DestroyID(id); 
        }
    
        private:

        ObjectID id;
        std::vector<ObjectID> children;
        ObjectID parent = ObjectIDBuilder().WithValue(-1).Build();

        friend class Actor;
    };
}