#include "actor.hpp"


namespace SHAME::Engine::ECS::Objects{
    
    using namespace Components;

    Actor::Actor(std::string name)
    {
        SetName(name);

        transform = make_shared<Transform>(this, this->components.size());
        components.push_back(transform);
    }

    std::shared_ptr<Component> Actor::AddComponentRaw(Component* rawComponent) {
        if (!rawComponent) {
            throw std::runtime_error("[ERROR] [ENGINE/ECS/ACTOR] Tried to add null component.");
        }

        // Wrap in shared_ptr
        std::shared_ptr<Component> component(rawComponent);

        // Set actor and component index (safely override if base constructor didn't set)
        component->parent = this;
        component->local_id = components.size();

        if (dynamic_cast<Transform*>(component.get())) {
            throw std::runtime_error("[ERROR] [ENGINE/ECS/ACTOR] An actor can only have one transform component.");
        }

        components.push_back(component);

        // Register in system component arrays
        if (auto light = std::dynamic_pointer_cast<Light>(component)) {
            light->SetLightIndex(level->lights.size());
            level->lights.push_back(light);
        }

        if (auto model = std::dynamic_pointer_cast<Model>(component)) {
            level->models.push_back(model);
        }

        if (auto transform = std::dynamic_pointer_cast<Transform>(component)) {
            level->transforms.push_back(transform);
        }

        if (auto physics = std::dynamic_pointer_cast<PhysicsBody>(component)) {
            level->physicsBodies.push_back(physics);
        }

        if (auto audio = std::dynamic_pointer_cast<AudioSource>(component)) {
            level->audioSources.push_back(audio);
        }

        if (auto script = std::dynamic_pointer_cast<Script>(component)) {
            level->scripts.push_back(script);

            uint32_t id = GetComponentIDInScene(components.size() - 1);

            Events::EventDispatcher::GetInstance().subscribeToComponent<Events::ContactAddedEvent>(id, [script](const Events::ContactAddedEvent& event) {
                script->OnContactAdded(event);
            });
            Events::EventDispatcher::GetInstance().subscribeToComponent<Events::ContactPersistedEvent>(id, [script](const Events::ContactPersistedEvent& event) {
                script->OnContactPersisted(event);
            });
            Events::EventDispatcher::GetInstance().subscribeToComponent<Events::ContactRemovedEvent>(id, [script](const Events::ContactRemovedEvent& event) {
                script->OnContactEnded(event);
            });
        }

        if (auto cam = std::dynamic_pointer_cast<Camera>(component)) {
            level->cameras.push_back(cam);
            Rendering::CameraManager::AddCamera(name, cam);
        }

        return component;
    }

    void Actor::AddChild(std::shared_ptr<Object> o)
    {
        Object::AddChild(o);
        if (std::shared_ptr<Actor> actorChild = std::dynamic_pointer_cast<Actor>(o)) {
            actorChild->SetLevel(this->level);
        }
    }

    void Actor::SetLevel(Levels::Level* lvl)
    {
        this->level = lvl;
    }
}