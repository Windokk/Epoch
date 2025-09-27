#pragma once

#include "engine/rendering/framebuffer/framebuffer.hpp"
#include "engine/rendering/light/light_manager.hpp"
#include "engine/rendering/mesh/mesh.hpp"

#define MAX_DIRECTIONAL_LIGHTS 3
#define CASCADES_PER_LIGHT 3
#define NUM_CASCADES (MAX_DIRECTIONAL_LIGHTS * CASCADES_PER_LIGHT)

namespace EPOCH::Engine::ECS::Components{
    class Camera;
}

namespace EPOCH::Engine::Rendering{

    class Shader;

    
    struct ShadowMap {
        LightData light;

        // Directional and spot
        GLuint fbo[CASCADES_PER_LIGHT] = {0};       // One FBO per cascade (only 1 used for spot)
        GLuint depthMap[CASCADES_PER_LIGHT] = {0};  // One shadow map per cascade
        GLuint resolveDepthMap[CASCADES_PER_LIGHT] = { 0 };
        glm::mat4 lightMatrix[CASCADES_PER_LIGHT];  // One matrix per cascade
        float cascadeSplits[CASCADES_PER_LIGHT];    // Only used for directional lights

        // Point
        GLuint depthCubemap = 0;              // Single cubemap array for point lights
        glm::mat4 shadowMatrices[6];          // 6 face matrices (point lights)
        int cubeArrayLayer = -1;

        int resolution = 1024;
    };

    class ShadowManager {
    public:
        void Init(int resolution);
        void RegisterLight(int lightIndex, std::shared_ptr<LightData> light);
        void ResolveShadowMaps();
        void UnregisterLight(int lightIndex);
        void RenderShadowMaps(const std::unordered_map<int, std::pair<glm::mat4, Rendering::Mesh *>> &meshes, std::shared_ptr<ECS::Components::Camera> cam);
        void BindShadowMaps(std::shared_ptr<EPOCH::Engine::Rendering::Material> material);

    private:
        std::vector<ShadowMap> shadowMaps;
        Shader dirShader, spotShader, pointShader;
        int shadowResolution = 0;
        GLuint cubeArrayTex;
        int pointLightCount = 0;
    };
}