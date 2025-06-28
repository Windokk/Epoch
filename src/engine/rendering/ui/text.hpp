#pragma once

#include "font.hpp"

#include "engine/rendering/utils.hpp"
#include "engine/rendering/shader/shader.hpp"

#include "engine/ecs/components/transform.hpp"

#include <string>

namespace SHAME::Engine::Rendering::UI
{
    using namespace ECS::Components;

    class Text{
        std::string text;
        glm::vec4 color;
        Font font;
        Transform transform;

        public:
        Text(Font& font, std::string text, glm::vec4 color, Transform transform);

        void Draw(Shader& shader);
    };
}