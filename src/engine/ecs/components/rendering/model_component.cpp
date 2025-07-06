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

    void Model::SetMaterial(std::shared_ptr<Rendering::Material> material)
    {
        if(!activated)
            return;
            
        this->mat = material;
        this->Update();
    }

    void Model::Update(){

        if(!activated)
            return;

        if (mat != nullptr && mesh != nullptr){
            Rendering::DrawCommand cmd;
            
            Transform& tr = parent->GetComponent<Transform>();

            cmd = mesh->CreateDrawCmd();

            cmd.mat = this->mat;
            cmd.id = parent->GetComponentIDInScene(local_id);
            cmd.tr = &tr;

            Rendering::Renderer::Submit(cmd, alreadySubmitted);

            alreadySubmitted = true;
        }
    }
}