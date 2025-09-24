#pragma once

#include "engine/ecs/components/core/component.hpp"

#include "engine/rendering/renderer/renderer.hpp"

namespace EPOCH::Engine::ECS::Components
{
    class Light : public Component{
        public:
            Light(Objects::Actor *parent, uint32_t local_id);

            void SetType(Rendering::LightType type);
            void SetIntensity(float intensity);
            void SetPosition(glm::vec3 postion);
            void SetDirection(glm::vec3 direction);
            void SetRadius(float radius);
            void SetColor(glm::vec3 color);
            void SetOuterCutoff(float cutoff);
            void SetInnerCuttof(float cutoff);
            void SetLightIndex(int index);
            void SetCastShadow(bool castShadows);

            Rendering::LightData GetData();
            
            void Destroy() override{
                Rendering::Renderer::lightMan->RemoveLight(lightIndex);
            }

        private:
            int lightIndex = -1;
            std::shared_ptr<Rendering::LightData> lightData;
    };
}