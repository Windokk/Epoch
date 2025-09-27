#pragma once

#include "engine/ecs/components/core/component.hpp"

#include "engine/rendering/mesh/mesh.hpp"

namespace EPOCH::Engine::ECS::Components
{
    class Model : public Component{
        public:
            Model(Objects::Actor *parent, uint32_t local_id);

            public:

            void SetMesh(std::shared_ptr<Rendering::Mesh> mesh);
            void UpdateReferenceInLevel();
            void SetMaterials(std::vector<std::shared_ptr<Rendering::Material>> &&materials);

            std::shared_ptr<Rendering::Mesh> GetMesh() { return mesh; }
            std::vector<std::shared_ptr<Rendering::Material>> GetMaterials() { return materials; }

            int GetMaterialsCount() { return materials.size(); }

            void Update();
            void RemoveFromDrawList();
        private:

            bool alreadySubmitted = false;

            std::shared_ptr<Rendering::Mesh> mesh;
            std::vector<std::shared_ptr<Rendering::Material>> materials;

    };
}