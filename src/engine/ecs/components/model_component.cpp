#include "model_component.hpp"

#include "engine/rendering/renderer/renderer.hpp"

#include "engine/ecs/objects/actors/actor.hpp"

namespace SHAME::Engine::ECS::Components{
    

    ModelComponent::ModelComponent(Objects::Actor *parent, uint32_t local_id) : Component(parent, local_id)
    {
    }

    void ModelComponent::SetMesh(std::shared_ptr<Rendering::Mesh> mesh)
    {
        this->mesh = mesh;
        this->Update();
    }
    
    void ModelComponent::SetMaterial(std::shared_ptr<Rendering::Material> material)
    {
        this->mat = material;
        this->Update();
    }

    void ModelComponent::Update(){
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