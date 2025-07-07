#pragma once

#include "engine/rendering/shader/shader.hpp"
#include "engine/rendering/texture/texture.hpp"

#include <variant>
#include <stdexcept>
#include <memory>

namespace SHAME::Engine::Rendering {

    using UniformValue = std::variant<float, int, glm::vec2, glm::vec3, glm::vec4, glm::mat4, Texture*>;

    class Material{

        public:
            Material(Filesystem::Path *objPath, bool castShadows);
            Material(std::shared_ptr<Shader> shader, bool castShadows);
            void Init(std::shared_ptr<Shader> shader);

            void SetParameter(const std::string &name, const UniformValue &value)
            {
                parameters[name] = value;
            }
    
            std::unordered_map<std::string, UniformValue>* GetParameters() { return &parameters; };
            Texture* GetTexture(TextureType type);
            void Use();
            void StopUsing();
            std::shared_ptr<Shader> shader;
            bool castShadows = true;

        private:
            std::unordered_map<std::string, UniformValue> parameters;

    };

}