#pragma once

#include "engine/rendering/utils.hpp"

#include <memory>

namespace SHAME::Engine::Rendering{

    enum class LightType{
        Directional = 0,
        Point = 1,
        Spot = 2
    };

    struct LightData {
        alignas(4) int type = 0;
        alignas(4) float intensity= 1.0f;
        alignas(16) glm::vec3 position = glm::vec3(0, 0, 0);
        alignas(16) glm::vec3 direction = glm::vec3(0, -1, 0);
        alignas(4) float radius = 100;
        alignas(16) glm::vec3 color = COL_RGB(1.0f);
        alignas(4) float innerCutoff = glm::cos(glm::radians(1.5f));
        alignas(4) float outerCutoff = glm::cos(glm::radians(7.5f));
        alignas(4) bool castShadow = true;

        glm::mat4 GetLightMatrix() const;
    };

    class LightManager {
        public:
            LightManager();

            ~LightManager();

            void Update(int updatedLight);

            void AddLight(int index, std::shared_ptr<LightData> light);

            void Clear();

            void RemoveLight(int lightIndex);

            int GetLightsCount();

        private:
            GLuint ssbo = 0;
            std::vector<std::shared_ptr<LightData>> lights;
    };


};