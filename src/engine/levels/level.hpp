#pragma once

#include <string>

#include "engine/rendering/renderer/renderer.hpp"

#include "engine/ecs/components/physics_component.hpp"
#include "engine/ecs/components/audio_source.hpp"

namespace SHAME::Engine::ECS{

    namespace Objects{
        class Actor;
    }

    class ObjectID;

    namespace Components{
        class Light;
    }
}

namespace SHAME::Engine::Levels{
 
    namespace Rendering{
        class Renderer;
    }

    class Level{

        std::vector<ECS::Objects::Actor*> rootActors;
        std::string name;
        std::string path;

        public:
        Level(std::string name);

        void Save(const std::string& filepath);

        void Render(Rendering::Renderer& renderer);
        void Clear();

        void AddActor(ECS::Objects::Actor* a);
        void RemoveActor(ECS::ObjectID id, bool recursive = false);
        ECS::Objects::Actor* GetActor(ECS::ObjectID id, bool recursive = false);
        std::vector<ECS::ObjectID> GetActorsID(bool recursive = false);
        std::vector<ECS::Objects::Actor*> GetRootActors() { return rootActors; };

        const std::string& GetName() const;
        void SetName(const std::string& name);

        std::vector<ECS::Components::Light*> lights;
        std::vector<ECS::Components::Transform*> transforms;
        std::vector<ECS::Components::ModelComponent*> models;
        std::vector<ECS::Components::PhysicsComponent*> physicsBodies;
        std::vector<ECS::Components::AudioSource*> audioSources;
        
    };

}
