#include "model_component.hpp"

#include "engine/rendering/renderer/renderer.hpp"

#include "engine/ecs/objects/actors/actor.hpp"

namespace SHAME::Engine::ECS::Components{
    

    Model::Model(Objects::Actor *parent, uint32_t local_id) : Component(parent, local_id)
    {
    }

    void Model::SetMesh(std::shared_ptr<Rendering::Mesh> mesh)
    {
        if(!activated)
            return;

        this->mesh = mesh;
        this->Update();
        UpdateReferenceInLevel();
    }
    
    void Model::UpdateReferenceInLevel()
    {
        if (!parent || !parent->level || !mesh || !activated)
        {
            return;
        }
        parent->level->meshes[parent->GetComponentIDInScene(local_id)] = { parent->transform->GetTransformMatrix(), mesh.get() };
    }

    void Model::SetMaterials(std::vector<std::shared_ptr<Rendering::Material>> materials)
    {
        if(!activated)
            return;
            
        this->materials = materials;
        this->Update();
    }

    void Model::Update(){

        if(!activated)
            return;

        if (materials.size() > 0 && mesh != nullptr){

            Transform* tr = &parent->GetComponent<Transform>();

            std::vector<Rendering::DrawCommand> cmds = mesh->CreateDrawCmds(tr, parent->GetComponentIDInScene(local_id), this->materials);

            Rendering::Renderer::Submit(cmds, alreadySubmitted);

            alreadySubmitted = true;
        }
    }
}