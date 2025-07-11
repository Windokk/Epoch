#pragma once

#include <string>

#include "engine/rendering/renderer/renderer.hpp"

#include "engine/ecs/components/physics/physics_body.hpp"
#include "engine/ecs/components/audio/audio_source.hpp"
#include "engine/ecs/components/rendering/camera.hpp"
#include "engine/ecs/components/core/script.hpp"

namespace SHAME::Engine::ECS{

    namespace Objects{
        class Actor;
    }

    class ObjectID;

    namespace Components{
        class Light;
    }
}

namespace SHAME::Engine::Rendering{
    class Renderer;
    class Mesh;
}

namespace SHAME::Engine::Levels{

    class Level{

        std::vector<std::shared_ptr<ECS::Objects::Actor>> rootActors;
        std::string name;
        std::string path;

        public:
        Level(std::string name);

        void Save(const std::string& filepath);

        void Clear();

        void Tick();

        void AddActor(std::shared_ptr<ECS::Objects::Actor> a);
        void RemoveActor(ECS::ObjectID id, bool recursive = false);
        std::shared_ptr<ECS::Objects::Actor> GetActor(ECS::ObjectID id, bool recursive = false);
        std::vector<ECS::ObjectID> GetActorsID(bool recursive = false);
        std::vector<std::shared_ptr<ECS::Objects::Actor>> GetRootActors() { return rootActors; };

        const std::string& GetName() const;
        void SetName(const std::string& name);

        std::vector<ECS::Components::Light*> lights;
        std::vector<ECS::Components::Transform*> transforms;
        std::vector<ECS::Components::Model*> models;
        std::vector<ECS::Components::PhysicsBody*> physicsBodies;
        std::vector<ECS::Components::AudioSource*> audioSources;
        std::vector<ECS::Components::Camera*> cameras;
        std::vector<ECS::Components::Script*> scripts;

        std::unordered_map<int, std::pair<glm::mat4, Rendering::Mesh*>> meshes;
        
    };

}
