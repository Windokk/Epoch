#pragma once

#include "engine/rendering/mesh/mesh.hpp"

#include "engine/ecs/components/component.hpp"

namespace SHAME::Engine::ECS::Components
{
    class ModelComponent : public Component{
        public:
            ModelComponent(Objects::Actor *parent, uint32_t local_id);

            public:
            void Update();

            void SetMesh(std::shared_ptr<Rendering::Mesh> mesh);
            void SetMaterial(std::shared_ptr<Rendering::Material> material);

            std::shared_ptr<Rendering::Mesh> GetMesh() { return mesh; }
            std::shared_ptr<Rendering::Material> GetMaterial() { return mat; }

        private:

            bool alreadySubmitted = false;

            std::shared_ptr<Rendering::Mesh> mesh;
            std::shared_ptr<Rendering::Material> mat;

    };
}