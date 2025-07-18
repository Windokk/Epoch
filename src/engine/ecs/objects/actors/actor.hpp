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
        
        std::vector<std::shared_ptr<Component>> components;        
        std::string name;
        public:
            Actor(std::string name);

            std::shared_ptr<Component> AddComponentRaw(Component *rawComponent);

            template <typename T>
            bool HasComponent();

            template <typename T>
            std::vector<std::shared_ptr<T>> GetComponents();

            template <typename T>
            std::shared_ptr<T> GetComponent(int k = 0);

            const std::vector<std::shared_ptr<Component>>& GetComponents() const {
                return components;
            }

            template <typename T>
            std::shared_ptr<T> AddComponent();

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

            void AddChild(std::shared_ptr<Object> o) override;

            void SetLevel(Levels::Level* lvl);

            std::shared_ptr<Transform> transform;
            Levels::Level* level;

            void Activate(){
                activated = true;
                for(auto& component : components){
                    component->Activate();
                }
            }

            void DeActivate(){
                activated = false;
                for(auto& component : components){
                    component->DeActivate();
                }
            }

            bool Active(){
                return activated;
            }

        protected:
            bool activated = true;
    };

    

    template <typename T>
    bool Actor::HasComponent() {
        if(!std::is_base_of<Component, T>::value){
            DEBUG_ERROR("T must inherit from Component");
        }

        for (const std::shared_ptr<Component>& component : components) {
            if (dynamic_cast<T*>(component.get())) {
                return true;
            }
        }
        return false;
    }

    template <typename T>
    std::vector<std::shared_ptr<T>> Actor::GetComponents(){
        if(!std::is_base_of<Component, T>::value){
            DEBUG_ERROR("T must inherit from Component");
        }

        std::vector<std::shared_ptr<T>> list;

        for (const auto& component : components) {
            if (auto casted = std::dynamic_pointer_cast<T>(component)) {
                list.push_back(casted);
            }
        }

        return list;
    } 

    template <typename T>
    std::shared_ptr<T> Actor::GetComponent(int k) {
        if(!std::is_base_of<Component, T>::value){
            DEBUG_ERROR("T must inherit from Component");
        }

        int n = 0;

        for (auto& component : components) {
            if (auto casted = std::dynamic_pointer_cast<T>(component)) {
                if(n != k){
                    n++;
                    continue;
                }
                else{
                    return casted;
                }
            }
        }
        DEBUG_ERROR(std::string("Failed to retrieve Component of type ") + typeid(T).name());
        return nullptr;
    }

    template <typename T>
    std::shared_ptr<T> Actor::AddComponent()
    {
        if(!std::is_base_of<Component, T>::value){
            DEBUG_ERROR("T must inherit from Component");
            return nullptr;
        }

        std::shared_ptr<T> component = std::make_shared<T>(this, components.size());
        
        if (std::is_base_of<Transform, T>::value) {
            DEBUG_ERROR("An actor can only have one transform component.");
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
            Rendering::CameraManager::AddCamera(name, component);
        }

        return component;
    }
}