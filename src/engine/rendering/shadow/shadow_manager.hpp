#pragma once

#include "engine/rendering/framebuffer/framebuffer.hpp"
#include "engine/rendering/light/light_manager.hpp"
#include "engine/rendering/mesh/mesh.hpp"

namespace SHAME::Engine::Rendering{

    class Shader;

    
    struct ShadowMap {
        GLuint fbo = 0;
        LightData light;
        GLuint depthMap = 0;
        GLuint depthCubemap = 0;
        glm::mat4 lightMatrix;
        glm::mat4 shadowMatrices[6];
        int resolution;
        int cubeArrayLayer = -1;
    };

    class ShadowManager {
    public:
        void Init(int resolution);
        void RegisterLight(int lightIndex, std::shared_ptr<LightData> light);
        void UnregisterLight(int lightIndex);
        void RenderShadowMaps(const std::unordered_map<int, std::pair<glm::mat4, Rendering::Mesh*>> &meshes);
        void BindShadowMaps(std::shared_ptr<SHAME::Engine::Rendering::Material> material);

    private:
        std::vector<ShadowMap> shadowMaps;
        Shader dirShader, spotShader, pointShader;
        int SHADOW_RES = 1024;
        GLuint cubeArrayTex;
        int pointLightCount = 0;
    };
}