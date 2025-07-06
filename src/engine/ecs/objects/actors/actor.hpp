#pragma once

#include "engine/ecs/objects/object.hpp"

#include "engine/ecs/components/core/transform.hpp"
#include "engine/ecs/components/rendering/light_component.hpp"
#include "engine/ecs/components/physics/physics_body.hpp"

#include "engine/rendering/camera/camera_manager.hpp"

#include "engine/levels/level.hpp"

#include "engine/events/event_system.hpp"

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

            Transform* transform;
            Levels::Level* level;
        private:
    };

    

    template <typename T>
    bool Actor::HasComponent() {
        if(!std::is_base_of<Component, T>::value){
            throw std::runtime_error("[ERROR] [ENGINE/ECS/ACTOR] T must inherit from Component");
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
            throw std::runtime_error("[ERROR] [ENGINE/ECS/ACTOR] T must inherit from Component");
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
            throw std::runtime_error("[ERROR] [ENGINE/ECS/ACTOR] T must inherit from Component");
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
            std::string("[ERROR] [ENGINE/ECS/ACTOR] : Failed to retrieve Component of type ") + typeid(T).name());
    }

    template <typename T>
    T* Actor::AddComponent()
    {
        if(!std::is_base_of<Component, T>::value){
            throw std::runtime_error("[ERROR] [ENGINE/ECS/ACTOR] T must inherit from Component");
        }

        T* component = new T(this, this->components.size());
        
        if (dynamic_cast<Transform*>(component)) {
            throw std::runtime_error("[ERROR] [ENGINE/ECS/ACTOR] Can only have one Transform per actor");
            return nullptr;
        }
        components.push_back(component);

        if constexpr (std::is_base_of<Light, T>::value) {
            component->SetLightIndex(level->lights.size());
            level->lights.push_back(component);
        }

        if constexpr (std::is_base_of<Model, T>::value) {
            level->models.push_back(component);
        }

        if constexpr (std::is_base_of<Transform, T>::value) {
            level->transforms.push_back(component);
        }

        if constexpr (std::is_base_of<PhysicsBody, T>::value) {
            level->physicsBodies.push_back(component);
        }

        if constexpr (std::is_base_of<AudioSource, T>::value) {
            level->audioSources.push_back(component);
        }

        if constexpr (std::is_base_of<Script, T>::value) {
            level->scripts.push_back(component);
            Events::EventDispatcher::GetInstance().subscribeToComponent<Events::ContactAddedEvent>(GetComponentIDInScene(components.size()-1), [component](const Events::ContactAddedEvent& event) {
                component->OnContactAdded(event);
            });
            Events::EventDispatcher::GetInstance().subscribeToComponent<Events::ContactPersistedEvent>(GetComponentIDInScene(components.size()-1), [component](const Events::ContactPersistedEvent& event) {
                component->OnContactPersisted(event);
            });
            Events::EventDispatcher::GetInstance().subscribeToComponent<Events::ContactRemovedEvent>(GetComponentIDInScene(components.size()-1), [component](const Events::ContactRemovedEvent& event) {
                component->OnContactEnded(event);
            });
        }

        if constexpr (std::is_base_of<Camera, T>::value) {
            level->cameras.push_back(component);
            component->Init(Rendering::Renderer::GetCurrentWidth(), Rendering::Renderer::GetCurrentHeight(), 0.1f, 100.0f);
            Rendering::CameraManager::AddCamera(name, std::make_shared<Camera>(*component));
        }

        return component;
    }
}