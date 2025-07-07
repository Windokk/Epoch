#pragma once

#include "engine/ecs/components/core/component.hpp"

#include "engine/rendering/mesh/mesh.hpp"

namespace SHAME::Engine::ECS::Components
{
    class Model : public Component{
        public:
            Model(Objects::Actor *parent, uint32_t local_id);

            public:
            void Update();

            void SetMesh(std::shared_ptr<Rendering::Mesh> mesh);
            void SetMeshAndMaterialFromPath(Filesystem::Path *path);
            void UpdateReferenceInLevel();
            void SetMaterial(std::shared_ptr<Rendering::Material> material);

            std::shared_ptr<Rendering::Mesh> GetMesh() { return mesh; }
            std::shared_ptr<Rendering::Material> GetMaterial() { return mat; }

        private:

            bool alreadySubmitted = false;

            std::shared_ptr<Rendering::Mesh> mesh;
            std::shared_ptr<Rendering::Material> mat;

    };
}