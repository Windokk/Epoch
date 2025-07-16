#include "light_component.hpp"

#include "engine/rendering/renderer/renderer.hpp"
#include "engine/ecs/components/core/transform.hpp"
#include "engine/ecs/objects/actors/actor.hpp"

namespace SHAME::Engine::ECS::Components{

    Light::Light(Objects::Actor *parent, uint32_t local_id) : Component(parent, local_id)
    {
        lightData = std::make_shared<Rendering::LightData>();

        stdd:shared_ptr<Transform> tr = parent->transform;

        lightData->position = glm::vec3(tr->GetPosition().x, tr->GetPosition().y, tr->GetPosition().z);
        lightData->direction = tr->GetForward();
    }

    /// @brief Set the light's type
    /// @param type The new type
    void Light::SetType(Rendering::LightType type)
    {
        if(!activated)
            return;

        lightData->type = (int)type;
        Rendering::Renderer::lightMan->Update(lightIndex);
    }

    /// @brief Set the light's intensity
    /// @param intensity The new intensity
    void Light::SetIntensity(float intensity)
    {
        if(!activated)
            return;

        lightData->intensity = intensity;
        Rendering::Renderer::lightMan->Update(lightIndex);
    }

    /// @brief Set the light's position in the world
    /// @param postion The new position (in world units)
    void Light::SetPosition(glm::vec3 postion)
    {
        if(!activated)
            return;

        lightData->position = postion;
        Rendering::Renderer::lightMan->Update(lightIndex);
    }

    /// @brief Set the light's direction (Only for spot and directionnal lights)
    /// @param direction The new direction (normalized)
    void Light::SetDirection(glm::vec3 direction)
    {
        if(!activated)
            return;

        lightData->direction = direction;
        Rendering::Renderer::lightMan->Update(lightIndex);
    }
    
    /// @brief Set the radius of the light (Only for spot and point lights)
    /// @param radius The new radius (in world units)
    void Light::SetRadius(float radius)
    {
        if(!activated)
            return;

        lightData->radius = radius;
        Rendering::Renderer::lightMan->Update(lightIndex);
    }

    /// @brief Sets the color of the light
    /// @param color The new color
    void Light::SetColor(glm::vec3 color)
    {
        if(!activated)
            return;

        lightData->color = color;
        Rendering::Renderer::lightMan->Update(lightIndex);
    }

    /// @brief Set the outer cuttof (Only for spot lights)
    /// @param cutoff The new cutoff, in degrees
    void Light::SetOuterCutoff(float cutoff)
    {
        if(!activated)
            return;

        lightData->outerCutoff = glm::cos(glm::radians(cutoff));
        Rendering::Renderer::lightMan->Update(lightIndex);
    }

    /// @brief Set the inner cuttof (Only for spot lights)
    /// @param cutoff The new cutoff, in degrees
    void Light::SetInnerCuttof(float cutoff)
    {
        if(!activated)
            return;

        lightData->innerCutoff = glm::cos(glm::radians(cutoff));
        Rendering::Renderer::lightMan->Update(lightIndex);
    }

    /// @brief Set the light's index in the scene
    /// @param index This new light's index
    void Light::SetLightIndex(int index)
    {
        if(lightIndex != -1 || !activated)
            return;
        
        Rendering::Renderer::lightMan->AddLight(index, lightData);
        Rendering::Renderer::lightMan->Update(index);
        lightIndex = index;
    }
    /// @brief Set wether this light should cast shadows
    /// @param castShadows true : cast shadows, false : doesn't cast shadows
    void Light::SetCastShadow(bool castShadows)
    {
        if(!activated)
            return;

        lightData->castShadow = castShadows;
        Rendering::Renderer::lightMan->Update(lightIndex);
    }

    /// @brief Getter for this light component's data
    /// @return A copy of this light component's data
    Rendering::LightData Light::GetData()
    {
        return *lightData.get();
    }
}