#pragma once

#include "engine/ecs/objects/object.hpp"

#include "engine/ecs/components/transform.hpp"
#include "engine/ecs/components/light_component.hpp"
#include "engine/ecs/components/physics_component.hpp"

#include "engine/levels/level.hpp"

#include <stdexcept>
#include <iostream>

namespace SHAME::Engine::ECS::Objects{

    using namespace Components;

    class Actor : public Object{
        
        std::vector<Component*> components;        
        std::string name;
        public:
            Actor(std::string name);
            ~Actor() { for(auto& c : components){ delete c; }}

            template <typename T>
            bool HasComponent();

            template <typename T>
            std::vector<T*> GetComponents();

            template <typename T>
            T &GetComponent(int k = 0);

            const std::vector<Component*>& GetComponents() const {
                return components;
            }

            template <typename T>
            T* AddComponent();

            void Destroy() override {
                
                for(auto& component : components){
                    component->Destroy();
                }

                Object::Destroy();
            }

            uint32_t GetComponentIDInScene(int componentIndex) { 
                uint32_t global_id = (static_cast<uint32_t>(id.GetAsInt()) << 16) | componentIndex;
                return global_id;
            }
            
            std::string GetName() { return name; }
            void SetName(std::string name) { this->name = name; }

            void AddChild(Object* o) override;

            void SetLevel(Levels::Level* lvl);

        private:
            Levels::Level* level;
    };

    

    template <typename T>
    bool Actor::HasComponent() {
        if(!std::is_base_of<Component, T>::value){
            throw std::runtime_error("[ERROR]  [ENGINE/ECS/ACTOR] T must inherit from Component");
        }

        for (Component* component : components) {
            if (dynamic_cast<T*>(component)) {
                return true;
            }
        }
        return false;
    }

    template <typename T>
    std::vector<T*> Actor::GetComponents(){
        if(!std::is_base_of<Component, T>::value){
            throw std::runtime_error("[ERROR]  [ENGINE/ECS/ACTOR] T must inherit from Component");
        }

        std::vector<T*> list;

        for (Component* component : components) {
            if (T* casted = dynamic_cast<T*>(component)) {
                list.push_back(casted);
            }
        }

        return list;
    } 

    template <typename T>
    T& Actor::GetComponent(int k) {
        if(!std::is_base_of<Component, T>::value){
            throw std::runtime_error("[ERROR]  [ENGINE/ECS/ACTOR] T must inherit from Component");
        }

        int n = 0;

        for (auto* component : components) {
            if (T* casted = dynamic_cast<T*>(component)) {
                if(n != k){
                    n++;
                    continue;
                }
                else{
                    return *casted;
                }
            }
        }
        throw std::runtime_error(
            std::string("[ERROR]  [ENGINE/ECS/ACTOR] : Failed to retrieve Component of type ") + typeid(T).name());
    }

    template <typename T>
    T* Actor::AddComponent()
    {
        if(!std::is_base_of<Component, T>::value){
            throw std::runtime_error("[ERROR]  [ENGINE/ECS/ACTOR] T must inherit from Component");
        }

        T* component = new T(this, this->components.size());
        components.push_back(component);

        if constexpr (std::is_base_of<Light, T>::value) {
        level->lights.push_back(component);
        }

        if constexpr (std::is_base_of<ModelComponent, T>::value) {
            level->models.push_back(component);
        }

        if constexpr (std::is_base_of<Transform, T>::value) {
            level->transforms.push_back(component);
        }

        if constexpr (std::is_base_of<PhysicsComponent, T>::value) {
            level->physicsBodies.push_back(component);
        }

        return component;
    }
}