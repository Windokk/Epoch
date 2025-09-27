#include "shadow_manager.hpp"

#include <iostream>

#include "engine/rendering/renderer/renderer.hpp"
#include "engine/ecs/components/rendering/camera.hpp"

namespace EPOCH::Engine::Rendering{
    
    float ComputeCascadeSplitDistance(int cascadeIndex, float nearPlane, float farPlane, int totalCascades)
    {
        float nd = static_cast<float>(cascadeIndex + 1) / static_cast<float>(totalCascades);

        // Uniform split
        float logSplit = nearPlane * std::pow(farPlane / nearPlane, nd);

        float uniformSplit = nearPlane + (farPlane - nearPlane) * nd;

        float splitDist = 0.2f * logSplit + (1 - 0.2f) * uniformSplit;

        return splitDist;
    }

    /// @brief Initializer for the shadow manager
    /// @param resolution The resolution to use for shadow maps
    void ShadowManager::Init(int resolution)
    {
        shadowResolution = resolution;
        dirShader = Shader(Filesystem::Path("engine_resources/shaders/shadows/shadow_dir.vert"), Filesystem::Path("engine_resources/shaders/shadows/shadow_dir.frag"));
        pointShader = Shader(Filesystem::Path("engine_resources/shaders/shadows/shadow_point.vert"), Filesystem::Path("engine_resources/shaders/shadows/shadow_point.frag"), Filesystem::Path("engine_resources/shaders/shadows/shadow_point.geom"));
        spotShader = Shader(Filesystem::Path("engine_resources/shaders/shadows/shadow_spot.vert"), Filesystem::Path("engine_resources/shaders/shadows/shadow_spot.frag"));
        shadowMaps.clear();
        pointLightCount = 0;

        glGenTextures(1, &cubeArrayTex);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, cubeArrayTex);
        int numPointLights = 12;
        glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT32F,
                    shadowResolution, shadowResolution, 6 * numPointLights);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
    }

    /// @brief Register/Replace a light for shadow mappings
    /// @param lightIndex The updated/new light's global index in the scene
    /// @param light The point to the light's data
    void ShadowManager::RegisterLight(int lightIndex, std::shared_ptr<LightData> light)
    {
        if (!light->castShadow)
            return;

        ShadowMap sm;
        sm.resolution = shadowResolution;
        sm.light = *light;

        if (light->type == int(LightType::Point)) {
            // --- Point Light (Cubemap Shadow) ---
            sm.cubeArrayLayer = pointLightCount;

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
        else if (light->type == int(LightType::Directional)) {
            // --- Directional Light (Cascaded Shadow Maps) ---
            for (int c = 0; c < CASCADES_PER_LIGHT; ++c) {
                glGenFramebuffers(1, &sm.fbo[c]);

                glGenTextures(1, &sm.depthMap[c]);
                glBindTexture(GL_TEXTURE_2D, sm.depthMap[c]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                        shadowResolution, shadowResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

                glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo[c]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sm.depthMap[c], 0);
                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        }
        else if (light->type == int(LightType::Spot)) {
            // --- Spot Light (Single 2D Shadow Map) ---
            glGenFramebuffers(1, &sm.fbo[0]);

            glGenTextures(1, &sm.depthMap[0]);
            glBindTexture(GL_TEXTURE_2D, sm.depthMap[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                        shadowResolution, shadowResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo[0]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sm.depthMap[0], 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            sm.lightMatrix[0] = light->GetLightMatrix();
        }


        // --- Replace or append the shadow map ---
        if (lightIndex < shadowMaps.size()) {
            ShadowMap& old = shadowMaps[lightIndex];

            // Clean up resources (if necessary)
            if (old.light.type == int(LightType::Directional)) {
                for (int c = 0; c < CASCADES_PER_LIGHT; ++c) {
                    if (glIsFramebuffer(old.fbo[c])) glDeleteFramebuffers(1, &old.fbo[c]);
                    if (glIsTexture(old.depthMap[c])) glDeleteTextures(1, &old.depthMap[c]);
                }
            }
            else if (old.light.type == int(LightType::Spot)) {
                if (glIsFramebuffer(old.fbo[0])) glDeleteFramebuffers(1, &old.fbo[0]);
                if (glIsTexture(old.depthMap[0])) glDeleteTextures(1, &old.depthMap[0]);
            }

            shadowMaps[lightIndex] = sm;
        } else {
            shadowMaps.push_back(sm);
            if (sm.light.type == int(LightType::Point))
                ++pointLightCount;
        }
    }

    void ShadowManager::ResolveShadowMaps()
    {
        GLuint resolveFBO;
        glGenFramebuffers(1, &resolveFBO);

        for (auto& sm : shadowMaps)
        {
            if (!sm.light.castShadow)
                continue;

            if (sm.light.type == static_cast<int>(LightType::Directional))
            {
                for (int c = 0; c < CASCADES_PER_LIGHT; ++c)
                {
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, sm.fbo[c]); // Multisampled
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFBO);
                    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sm.resolveDepthMap[c], 0);
                    
                    glBlitFramebuffer(0, 0, shadowResolution, shadowResolution,
                                    0, 0, shadowResolution, shadowResolution,
                                    GL_DEPTH_BUFFER_BIT, GL_NEAREST);
                }
            }
            else if (sm.light.type == static_cast<int>(LightType::Spot))
            {
                glBindFramebuffer(GL_READ_FRAMEBUFFER, sm.fbo[0]); // Multisampled
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFBO);
                glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sm.resolveDepthMap[0], 0);

                glBlitFramebuffer(0, 0, shadowResolution, shadowResolution,
                                0, 0, shadowResolution, shadowResolution,
                                GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &resolveFBO);
    }

    /// @brief Remove a light from shadow mappings
    /// @param lightIndex The light's global index in the scene
    void ShadowManager::UnregisterLight(int lightIndex)
    {
        if (lightIndex < 0 || lightIndex >= static_cast<int>(shadowMaps.size()))
            return;

        ShadowMap& removedSM = shadowMaps[lightIndex];

        // ---- Clean up Directional Light Resources (multiple cascades) ----
        if (removedSM.light.type == static_cast<int>(LightType::Directional)) {
            for (int c = 0; c < CASCADES_PER_LIGHT; ++c) {
                if (glIsFramebuffer(removedSM.fbo[c]))
                    glDeleteFramebuffers(1, &removedSM.fbo[c]);

                if (glIsTexture(removedSM.depthMap[c]))
                    glDeleteTextures(1, &removedSM.depthMap[c]);
            }
        }

        // ---- Clean up Spot Light Resources (single map) ----
        else if (removedSM.light.type == static_cast<int>(LightType::Spot)) {
            if (glIsFramebuffer(removedSM.fbo[0]))
                glDeleteFramebuffers(1, &removedSM.fbo[0]);

            if (glIsTexture(removedSM.depthMap[0]))
                glDeleteTextures(1, &removedSM.depthMap[0]);
        }

        // ---- Special Case: Point Light (cube map layer) ----
        else if (removedSM.light.type == static_cast<int>(LightType::Point)) {
            int removedLayer = removedSM.cubeArrayLayer;

            // Shift down cube array layer indices for other point lights
            for (auto& sm : shadowMaps) {
                if (sm.light.type == static_cast<int>(LightType::Point) &&
                    sm.cubeArrayLayer > removedLayer) {
                    --sm.cubeArrayLayer;
                }
            }

            --pointLightCount;

            // Note: depth cubemap is shared (likely a cube array), so we don't delete texture here
        }

        // ---- Remove from list ----
        shadowMaps.erase(shadowMaps.begin() + lightIndex);
    }

    /// @brief Render the scene into each shadow map
    /// @param meshes The meshes to render. All other meshes will be occluded from the shadow pass
    void ShadowManager::RenderShadowMaps(const std::unordered_map<int, std::pair<glm::mat4, Rendering::Mesh*>> &meshes, std::shared_ptr<ECS::Components::Camera> cam)
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);

        for (auto& sm : shadowMaps)
        {
            LightData* light = &sm.light;

            if (!light->castShadow)
                continue;

            if (light->type == static_cast<int>(LightType::Point))
            {
                glViewport(0, 0, sm.resolution, sm.resolution);
                glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo[0]);  // Reused FBO

                pointShader.Activate();

                for (int face = 0; face < 6; ++face)
                {
                    // Attach face of cubemap array for current point light layer
                    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                            cubeArrayTex, 0, sm.cubeArrayLayer * 6 + face);

                    glClear(GL_DEPTH_BUFFER_BIT);

                    pointShader.setMat4("shadowMatrices[0]", sm.shadowMatrices[face]);
                    pointShader.setFloat("farPlane", light->radius);
                    pointShader.setVec3("lightPos", light->position);

                    for (const auto& [_, pair] : meshes)
                    {
                        pointShader.setMat4("model", pair.first);
                        pair.second->DrawWithoutMaterial();
                    }
                }

                pointShader.Deactivate();
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            else if (light->type == static_cast<int>(LightType::Spot))
            {
                glViewport(0, 0, sm.resolution, sm.resolution);
                glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo[0]);
                glClear(GL_DEPTH_BUFFER_BIT);

                spotShader.Activate();
                
                spotShader.setMat4("lightSpaceMatrix", sm.lightMatrix[0]);

                for (const auto& [_, pair] : meshes)
                {
                    spotShader.setMat4("model", pair.first);
                    pair.second->DrawWithoutMaterial();
                }

                spotShader.Deactivate();
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            else if (light->type == static_cast<int>(LightType::Directional))
            {
                // Render each cascade individually
                dirShader.Activate();

                for (int c = 0; c < CASCADES_PER_LIGHT; ++c)
                {
                    glViewport(0, 0, sm.resolution, sm.resolution);
                    glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo[c]);
                    glClear(GL_DEPTH_BUFFER_BIT);

                    sm.cascadeSplits[c] = ComputeCascadeSplitDistance(c, cam->nearPlane, cam->farPlane, CASCADES_PER_LIGHT);
                    float splitNear = c == 0 ? cam->nearPlane : sm.cascadeSplits[c - 1];
                    float splitFar  = sm.cascadeSplits[c];
                    sm.lightMatrix[c] = light->GetLightMatrix(cam->GetView(), cam->fov, cam->GetSize().x/cam->GetSize().y, splitNear, splitFar, shadowResolution);

                    dirShader.setMat4("lightSpaceMatrix", sm.lightMatrix[c]);

                    for (const auto& [_, pair] : meshes)
                    {
                        dirShader.setMat4("model", pair.first);
                        pair.second->DrawWithoutMaterial();
                    }

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                }

                dirShader.Deactivate();
            }
        }

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
    }

    /// @brief Bind the shadow maps to a material
    /// @param material The material to bind shadow maps to
    void ShadowManager::BindShadowMaps(std::shared_ptr<EPOCH::Engine::Rendering::Material> material)
    {
        constexpr int MAX_SHADOW_LIGHTS = 16;
        constexpr int MAX_SPOT_LIGHTS = 12;
        constexpr int MAX_POINT_LIGHTS = 12;

        int textureUnit = 10;  // Start binding textures from unit 10

        // Initialize all shadow map references and matrices to safe defaults
        for (int i = 0; i < NUM_CASCADES; ++i) {
            material->SetParameter("shadow_dirShadowMaps[" + std::to_string(i) + "]", 0);
            material->SetParameter("shadow_dirLightSpaceMatrices[" + std::to_string(i) + "]", glm::mat4(1.0f));
            material->SetParameter("cascadeSplits[" + std::to_string(i) + "]", 1.0f); // Default to max depth
        }

        for (int i = 0; i < MAX_SPOT_LIGHTS; ++i) {
            material->SetParameter("shadow_spotShadowMaps[" + std::to_string(i) + "]", 0);
            material->SetParameter("shadow_spotLightSpaceMatrices[" + std::to_string(i) + "]", glm::mat4(1.0f));
        }

        for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
            material->SetParameter("shadow_pointLightFarPlanes[" + std::to_string(i) + "]", 0.0f);
        }

        int cascadeIndex = 0;
        int spotIndex = 0;

        for (const auto& sm : shadowMaps)
        {
            const LightData& light = sm.light;

            if (!light.castShadow)
                continue;

            // Handle Directional Lights (with cascades)
            if (light.type == static_cast<int>(LightType::Directional))
            {
                for (int c = 0; c < CASCADES_PER_LIGHT; ++c)
                {
                    if (cascadeIndex >= NUM_CASCADES || textureUnit >= 32)
                        break;

                    glActiveTexture(GL_TEXTURE0 + textureUnit);
                    glBindTexture(GL_TEXTURE_2D, sm.depthMap[c]);

                    material->SetParameter("shadow_dirShadowMaps[" + std::to_string(cascadeIndex) + "]", textureUnit);
                    material->SetParameter("shadow_dirLightSpaceMatrices[" + std::to_string(cascadeIndex) + "]", sm.lightMatrix[c]);
                    material->SetParameter("cascadeSplits[" + std::to_string(cascadeIndex) + "]", sm.cascadeSplits[c]);
                    
                    ++cascadeIndex;
                    ++textureUnit;
                }
            }
            // Spot lights
            else if (light.type == static_cast<int>(LightType::Spot))
            {
                if (spotIndex >= MAX_SPOT_LIGHTS || textureUnit >= 32)
                    break;

                glActiveTexture(GL_TEXTURE0 + textureUnit);
                glBindTexture(GL_TEXTURE_2D, sm.depthMap[0]);

                material->SetParameter("shadow_spotShadowMaps[" + std::to_string(spotIndex) + "]", textureUnit);
                material->SetParameter("shadow_spotLightSpaceMatrices[" + std::to_string(spotIndex) + "]", sm.lightMatrix[0]);

                ++spotIndex;
                ++textureUnit;
            }
            // Point lights
            else if (light.type == static_cast<int>(LightType::Point))
            {
                if (sm.cubeArrayLayer >= MAX_POINT_LIGHTS)
                    continue;

                material->SetParameter("shadow_pointLightFarPlanes[" + std::to_string(sm.cubeArrayLayer) + "]", light.radius);
            }
        }

        // Bind cube map array texture (point lights)s
        constexpr int cubeArrayUnit = 31;
        glActiveTexture(GL_TEXTURE0 + cubeArrayUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, cubeArrayTex);
        material->SetParameter("shadow_pointShadowMapArray", cubeArrayUnit);
    }
}