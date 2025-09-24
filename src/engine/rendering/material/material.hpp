#pragma once

#include "engine/rendering/shader/shader.hpp"
#include "engine/rendering/texture/texture.hpp"

#include <variant>
#include <stdexcept>
#include <memory>

namespace EPOCH::Engine::Rendering {

    using UniformValue = std::variant<float, int, glm::vec2, glm::vec3, glm::vec4, glm::mat4, std::pair<Rendering::TextureType,std::shared_ptr<Texture>>>;

    class Material{

        public:
            Material();
            void Init(std::shared_ptr<Shader> shader, bool castShadows);

            void SetParameter(const std::string &name, const UniformValue &value)
            {
                parameters[name] = value;
            }
    
            std::unordered_map<std::string, UniformValue>* GetParameters() { return &parameters; };
            std::shared_ptr<Texture> GetTexture(TextureType type);
            void Use();
            void StopUsing();
            std::shared_ptr<Shader> shader;
            bool castShadows = true;

        private:
            std::unordered_map<std::string, UniformValue> parameters;

    };

}