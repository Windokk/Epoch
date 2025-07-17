#include "shadow_manager.hpp"

#include <iostream>

#include "engine/rendering/renderer/renderer.hpp"

namespace SHAME::Engine::Rendering{
    
    /// @brief Initializer for the shadow manager
    /// @param resolution The resolution to use for shadow maps
    void ShadowManager::Init(int resolution)
    {
        SHADOW_RES = resolution;
        dirShader = Shader(Filesystem::Path("engine_resources/shaders/shadows/shadow_dir.vert"), Filesystem::Path("engine_resources/shaders/shadows/shadow_dir.frag"));
        pointShader = Shader(Filesystem::Path("engine_resources/shaders/shadows/shadow_point.vert"), Filesystem::Path("engine_resources/shaders/shadows/shadow_point.frag"), Filesystem::Path("engine_resources/shaders/shadows/shadow_point.geom"));
        spotShader = Shader(Filesystem::Path("engine_resources/shaders/shadows/shadow_spot.vert"), Filesystem::Path("engine_resources/shaders/shadows/shadow_spot.frag"));
        shadowMaps.clear();
        pointLightCount = 0;

        glGenTextures(1, &cubeArrayTex);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, cubeArrayTex);
        int numPointLights = 16;
        glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT32F,
                    SHADOW_RES, SHADOW_RES, 6 * numPointLights);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    /// @brief Register/Replace a light for shadow mappings
    /// @param lightIndex The updated/new light's global index in the scene
    /// @param light The point to the light's data
    void ShadowManager::RegisterLight(int lightIndex, std::shared_ptr<LightData> light)
    {
        if (!light->castShadow)
            return;

        ShadowMap sm;
        sm.resolution = SHADOW_RES;
        sm.light = *light.get();

        glGenFramebuffers(1, &sm.fbo);

        if (light->type == int(LightType::Point))
        {
            // Store layer index
            sm.cubeArrayLayer = pointLightCount;

            // Build 6 view-projection matrices
            float near = 0.1f;
            float far = light->radius;
            glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, near, far);
            glm::vec3 pos = light->position;

            sm.shadowMatrices[0] = proj * glm::lookAt(pos, pos + glm::vec3( 1,  0,  0), glm::vec3(0, -1,  0));
            sm.shadowMatrices[1] = proj * glm::lookAt(pos, pos + glm::vec3(-1,  0,  0), glm::vec3(0, -1,  0));
            sm.shadowMatrices[2] = proj * glm::lookAt(pos, pos + glm::vec3( 0,  1,  0), glm::vec3(0,  0,  1));
            sm.shadowMatrices[3] = proj * glm::lookAt(pos, pos + glm::vec3( 0, -1,  0), glm::vec3(0,  0, -1));
            sm.shadowMatrices[4] = proj * glm::lookAt(pos, pos + glm::vec3( 0,  0,  1), glm::vec3(0, -1,  0));
            sm.shadowMatrices[5] = proj * glm::lookAt(pos, pos + glm::vec3( 0,  0, -1), glm::vec3(0, -1,  0));
        }
        else
        {
            // --- Directional / Spot Light (2D Shadow Map) ---
            glGenTextures(1, &sm.depthMap);
            glBindTexture(GL_TEXTURE_2D, sm.depthMap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                        SHADOW_RES, SHADOW_RES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

            glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sm.depthMap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        sm.lightMatrix = light->GetLightMatrix();

        if (lightIndex < shadowMaps.size())
        {
            ShadowMap& old = shadowMaps[lightIndex];

            // Free FBO
            if (glIsFramebuffer(old.fbo))
                glDeleteFramebuffers(1, &old.fbo);

            // Free depth texture if it's 2D map
            if (old.light.type != static_cast<int>(LightType::Point) && glIsTexture(old.depthMap))
                glDeleteTextures(1, &old.depthMap);

            shadowMaps[lightIndex] = sm; // Replace

        }
        else
        {
            shadowMaps.push_back(sm); // Add new
            if(sm.light.type == static_cast<int>(LightType::Point)){
                ++pointLightCount;
            }
        }
            
    }

    /// @brief Remove a light from shadow mappings
    /// @param lightIndex The light's global index in the scene
    void ShadowManager::UnregisterLight(int lightIndex)
    {
        if (lightIndex < 0 || lightIndex >= shadowMaps.size())
            return;

        ShadowMap& removedSM = shadowMaps[lightIndex];

        // Delete associated OpenGL resources
        if (glIsFramebuffer(removedSM.fbo))
            glDeleteFramebuffers(1, &removedSM.fbo);

        if (removedSM.light.type != static_cast<int>(LightType::Point) && glIsTexture(removedSM.depthMap))
            glDeleteTextures(1, &removedSM.depthMap);

        // Special handling for point lights (with cube map array)
        if (removedSM.light.type == static_cast<int>(LightType::Point))
        {
            int removedLayer = removedSM.cubeArrayLayer;

            // Shift down layers of all later point lights
            for (auto& sm : shadowMaps)
            {
                if (sm.light.type == static_cast<int>(LightType::Point) && sm.cubeArrayLayer > removedLayer)
                {
                    --sm.cubeArrayLayer;
                }
            }

            --pointLightCount;
        }

        // Remove the shadow map from the vector
        shadowMaps.erase(shadowMaps.begin() + lightIndex);
    }

    /// @brief Render the scene into each shadow map
    /// @param meshes The meshes to render. All other meshes will be occluded from the shadow pass
    void ShadowManager::RenderShadowMaps(const std::unordered_map<int, std::pair<glm::mat4, Rendering::Mesh*>> &meshes)
    {
        glEnable(GL_DEPTH_TEST);

        for (const auto& sm : shadowMaps)
        {
            const LightData& light = sm.light;

            if (!light.castShadow)
                continue;

            if (light.type == static_cast<int>(LightType::Point))
            {
                glViewport(0, 0, sm.resolution, sm.resolution);
                glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo);

                pointShader.Activate();
                for (int face = 0; face < 6; ++face)
                {
                    // Attach the cube map array layer slice for this point light and face
                    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                            cubeArrayTex, 0, sm.cubeArrayLayer * 6 + face);
                    glClear(GL_DEPTH_BUFFER_BIT);

                    pointShader.setMat4("shadowMatrices[0]", sm.shadowMatrices[face]);
                    pointShader.setFloat("farPlane", light.radius);
                    pointShader.setVec3("lightPos", light.position);

                    for (auto& mesh : meshes)
                    {
                        pointShader.setMat4("model", mesh.second.first);
                        mesh.second.second->DrawWithoutMaterial();
                    }
                }
                pointShader.Deactivate();

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            else if (light.type == static_cast<int>(LightType::Directional))
            {
                // Directional light
                glViewport(0, 0, sm.resolution, sm.resolution);
                glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo);
                glClear(GL_DEPTH_BUFFER_BIT);

                dirShader.Activate();
                dirShader.setMat4("lightSpaceMatrix", sm.lightMatrix);
                for (auto mesh : meshes)
                {
                    dirShader.setMat4("model", mesh.second.first);
                    mesh.second.second->DrawWithoutMaterial();
                }

                dirShader.Deactivate();

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            else if (light.type == static_cast<int>(LightType::Spot))
            {
                // Spot light
                glViewport(0, 0, sm.resolution, sm.resolution);
                glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo);
                glClear(GL_DEPTH_BUFFER_BIT);

                spotShader.Activate();
                spotShader.setMat4("lightSpaceMatrix", sm.lightMatrix);

                for (auto mesh : meshes)
                {
                    spotShader.setMat4("model", mesh.second.first);
                    mesh.second.second->DrawWithoutMaterial();
                }

                spotShader.Deactivate();

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        }

        glDisable(GL_DEPTH_TEST);
    }

    /// @brief Bind the shadow maps to a material
    /// @param material The material to bind shadow maps to
    void ShadowManager::BindShadowMaps(std::shared_ptr<SHAME::Engine::Rendering::Material> material)
    {
        constexpr int MAX_SHADOW_LIGHTS = 16;
        int textureUnit = 10;

        // Set default 0s for directional and spot shadow maps as before
        for (int i = 0; i < MAX_SHADOW_LIGHTS; ++i)
        {
            material->SetParameter("shadow_dirShadowMaps[" + std::to_string(i) + "]", 0);
            material->SetParameter("shadow_spotShadowMaps[" + std::to_string(i) + "]", 0);
            material->SetParameter("shadow_pointLightFarPlanes[" + std::to_string(i) + "]", 0.0f);
            material->SetParameter("shadow_lightSpaceMatrices[" + std::to_string(i) + "]", glm::mat4(1.0f));
        }

        // Bind directional and spot shadow maps individually
        for (size_t i = 0; i < shadowMaps.size(); ++i)
        {
            const ShadowMap& sm = shadowMaps[i];
            const LightData& light = sm.light;

            if (!light.castShadow)
                continue;

            if (textureUnit >= 32)
            {
                std::cerr << "[ERROR] [ENGINE/RENDERING/SHADOWS] : Exceeded max number of active texture units (32)." << std::endl;
                break;
            }

            if (light.type == static_cast<int>(LightType::Directional))
            {
                glActiveTexture(GL_TEXTURE0 + textureUnit);
                glBindTexture(GL_TEXTURE_2D, sm.depthMap);
                material->SetParameter("shadow_dirShadowMaps[" + std::to_string(i) + "]", textureUnit);
                material->SetParameter("shadow_lightSpaceMatrices[" + std::to_string(i) + "]", sm.lightMatrix);
                ++textureUnit;
            }
            else if (light.type == static_cast<int>(LightType::Spot))
            {
                glActiveTexture(GL_TEXTURE0 + textureUnit);
                glBindTexture(GL_TEXTURE_2D, sm.depthMap);
                material->SetParameter("shadow_spotShadowMaps[" + std::to_string(i) + "]", textureUnit);
                material->SetParameter("shadow_lightSpaceMatrices[" + std::to_string(i) + "]", sm.lightMatrix);
                ++textureUnit;
            }
        }

        // Bind the cube map array once to a fixed texture unit (say 15)
        const int cubeArrayUnit = 31;
        glActiveTexture(GL_TEXTURE0 + cubeArrayUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, cubeArrayTex);
        material->SetParameter("shadow_pointShadowMapArray", cubeArrayUnit);

        // Set far planes for all point lights for shader usage
        for (const auto& pair : shadowMaps)
        {
            if (pair.light.type == static_cast<int>(LightType::Point))
                material->SetParameter("shadow_pointLightFarPlanes[" + std::to_string(pair.cubeArrayLayer) + "]", pair.light.radius);
        }
    }

}