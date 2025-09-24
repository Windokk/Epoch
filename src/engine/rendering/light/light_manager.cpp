#include "light_manager.hpp"

#include <type_traits>
#include <iostream>

#include "engine/rendering/renderer/renderer.hpp"

namespace EPOCH::Engine::Rendering{
    
    LightManager::LightManager()
    {
        glGenBuffers(1, &ssbo);
    }

    LightManager::~LightManager()
    {
        glDeleteBuffers(1, &ssbo);
    }

    /// @note Only call this AFTER modifying the light data
    /// @brief Update the light system's storage buffer, and shadow maps
    /// @param updatedLight The global index (scene-relative) of the modified light
    void LightManager::Update(int updatedLight){

        if(lights.size() != 0){
            std::vector<LightData> flatLights;
            flatLights.reserve(lights.size());

            for (std::shared_ptr<LightData> light : lights) {
                if (light) {
                    flatLights.push_back(*light.get());
                }
            }

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightData) * flatLights.size(), flatLights.data(), GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
        }

        if(updatedLight == -1)
            return;

        Renderer::shadowMan->RegisterLight(updatedLight, lights[updatedLight]);
    }

    /// @brief Add a light to the renderer
    /// @param index The global index (scene-relative) of the light
    /// @param light The light's parameters
    void LightManager::AddLight(int index, std::shared_ptr<LightData> light)
    {
        lights.push_back(light);
    }

    /// @brief Remove all lights from the renderer (and their associated shadow maps)
    void LightManager::Clear()
    {
        for (int i = 0; i < lights.size(); ++i)
        {
            if (lights[i] && lights[i]->castShadow)
            {
                Renderer::shadowMan->UnregisterLight(i);
            }
        }

        lights.clear();
    }

    /// @brief Remove light from renderer (and its associated shadow map)
    /// @param lightIndex The global (scene-relative) light index to remove
    void LightManager::RemoveLight(int lightIndex)
    {
        if (lightIndex < 0 || lightIndex >= lights.size())
            return;

        if (lights[lightIndex] && lights[lightIndex]->castShadow)
        {
            Renderer::shadowMan->UnregisterLight(lightIndex);
        }

        lights.erase(lights.begin() + lightIndex);

        for (int i = 0; i < lights.size(); ++i)
        {
            if (lights[i] && lights[i]->castShadow)
            {
                Renderer::shadowMan->RegisterLight(i, lights[i]);
            }
        }
    }

    /// @brief Getter for lights count
    /// @return The total number of lights in the renderer
    int LightManager::GetLightsCount()
    {
        return lights.size();
    }
    
    /// @brief Getter for lights matrices
    /// @return The view-projection matrix from the light's point of view
    glm::mat4 LightData::GetLightMatrix() const
    {
        if (type == static_cast<int>(LightType::Directional))
        {
            float orthoSize = 10.0f;
            glm::mat4 proj = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 0.1f, radius);

            glm::vec3 lightDir = glm::normalize(direction);
            glm::vec3 lightPos = -lightDir * 20.0f;

            glm::vec3 up = glm::abs(glm::dot(lightDir, glm::vec3(0, 1, 0))) > 0.99f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
            glm::mat4 view = glm::lookAt(lightPos, glm::vec3(0), up);

            return proj * view;
        }
        else if (type == static_cast<int>(LightType::Spot))
        {
            float orthoSize = 10.0f;
            glm::mat4 proj = glm::perspective(glm::radians(60.0f), static_cast<float>(Renderer::GetCurrentWidth()/Renderer::GetCurrentHeight()), 0.1f, radius);

            glm::vec3 lightDir = glm::normalize(direction);
            glm::vec3 lightPos = position;

            glm::vec3 up = glm::abs(glm::dot(lightDir, glm::vec3(0, 1, 0))) > 0.99f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
            glm::mat4 view = glm::lookAt(lightPos, lightPos+lightDir, up);

            return proj * view;
        }

        // For point lights : returns identity matrix
        return glm::mat4(1.0f);
    }
}