#pragma once

#include "engine/rendering/utils.hpp"

#include <memory>

namespace SHAME::Engine::Rendering{

    enum class LightType{
        Directional,
        Point,
        Spot
    };

    struct GPULight {
        alignas(4) int type = 0;
        alignas(4) float intensity= 1.0f;
        alignas(16) glm::vec3 position = glm::vec3(0, 0, 0);
        alignas(16) glm::vec3 direction = glm::vec3(0, -1, 0);
        alignas(4) float radius = 1;
        alignas(16) glm::vec3 color = COL_RGB(1.0f);
        alignas(4) float innerCutoff;
        alignas(4) float outerCutoff;
        alignas(4) float _padding;
    };

    class LightManager {
        public:
            LightManager();

            ~LightManager();

            void Update();

            void AddLight(std::shared_ptr<GPULight> light);

            void Clear();

            int GetLightsCount();

        private:
            GLuint ssbo = 0;
            std::vector<std::shared_ptr<GPULight>> lights;
    };


};