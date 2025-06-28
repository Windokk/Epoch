#include "light_component.hpp"

#include "engine/rendering/renderer/renderer.hpp"
#include "engine/ecs/components/transform.hpp"
#include "engine/ecs/objects/actors/actor.hpp"

namespace SHAME::Engine::ECS::Components{
    
    Light::Light(Objects::Actor *parent, uint32_t local_id) : Component(parent, local_id)
    {
        lightData = std::make_shared<Rendering::GPULight>();

        Transform tr = parent->GetComponent<Transform>();

        lightData->position = glm::vec3(tr.GetPosition().x, tr.GetPosition().y, tr.GetPosition().z);
        lightData->direction = glm::eulerAngles(tr.GetRotation());

        Rendering::Renderer::lightman->AddLight(lightData);
        Rendering::Renderer::lightman->Update();
    }

    void Light::SetType(Rendering::LightType type)
    {
        lightData->type = (int)type;
        Rendering::Renderer::lightman->Update();
    }

    void Light::SetIntensity(float intensity)
    {
        lightData->intensity = intensity;
        Rendering::Renderer::lightman->Update();
    }

    void Light::SetPosition(glm::vec3 postion)
    {
        lightData->position = postion;
        Rendering::Renderer::lightman->Update();
    }

    void Light::SetDirection(glm::vec3 direction)
    {
        lightData->direction = direction;
        Rendering::Renderer::lightman->Update();
    }

    void Light::SetRadius(float radius)
    {
        lightData->radius = radius;
        Rendering::Renderer::lightman->Update();
    }

    void Light::SetColor(glm::vec3 color)
    {
        lightData->color = color;
        Rendering::Renderer::lightman->Update();
    }

    void Light::SetOuterCutoff(float cutoff)
    {
        lightData->outerCutoff = cutoff;
        Rendering::Renderer::lightman->Update();
    }

    void Light::SetInnerCuttof(float cutoff)
    {
        lightData->innerCutoff = cutoff;
        Rendering::Renderer::lightman->Update();
    }

    Rendering::GPULight Light::GetData()
    {
        return *lightData.get();
    }
}