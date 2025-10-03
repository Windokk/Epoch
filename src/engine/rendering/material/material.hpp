#pragma once

#include "engine/rendering/shader/shader.hpp"
#include "engine/rendering/texture/texture.hpp"

#include <variant>
#include <stdexcept>
#include <memory>

namespace Epoch::Engine::Rendering {

    using UniformValue = std::variant<float, int, glm::vec2, glm::vec3, glm::vec4, glm::mat4, std::shared_ptr<Texture>>;

    class Material{

        public:
            Material();
            void Init(std::shared_ptr<Shader> shader, bool recievesShadows);

            void SetParameter(const std::string &name, const UniformValue &value)
            {
                parameters[name] = value;
            }
    
            std::map<std::string, UniformValue>* GetParameters() { return &parameters; };
            std::shared_ptr<Texture> GetTexture(std::string name);
            void Use();
            void StopUsing();
            std::shared_ptr<Shader> shader;
            bool recievesShadows = false;

        private:
            std::map<std::string, UniformValue> parameters;

    };

}