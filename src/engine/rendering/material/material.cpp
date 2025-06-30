#include "material.hpp"

namespace SHAME::Engine::Rendering{

    UniformValue DefaultValueForType(GLenum type) {
        switch (type) {
            case GL_FLOAT: return 0.0f;
            case GL_INT: return 0;
            case GL_FLOAT_VEC3: return glm::vec3(0.0f);
            case GL_FLOAT_VEC4: return glm::vec4(0.0f);
            case GL_FLOAT_MAT4: return glm::mat4(1.0f);
            case GL_SAMPLER_2D: return static_cast<Texture*>(nullptr);;
            default: throw std::runtime_error("[ERROR] [ENGINE/RENDERING/MATERIAL] : Failed to query default variable type");
        }
    }

    Material::Material(std::shared_ptr<Shader> shader)
    {
        Init(shader);
    }
 
    void Material::Init(std::shared_ptr<Shader> shader)
    {
        this->shader = shader;

        auto uniforms = this->shader->GetActiveUniforms();
        for (const auto& uniform : uniforms) {
            parameters[uniform.name] = DefaultValueForType(uniform.type);
        }
    }

    Texture* Material::GetTexture(int index)
    {
        std::vector<Texture*> textures;

        for (const auto& [name, value] : parameters)
        {
            if (std::holds_alternative<Texture*>(value))
            {
                Texture* val = std::get<Texture*>(value);
                textures.push_back(val);
            }
        }
        
        return textures[index];
    }

    void Material::Use()
    {
        shader->Activate();

        int textureUnit = 0;

        for (const auto& [name, value] : parameters)
        {
            if (std::holds_alternative<float>(value)){
                shader->setFloat(name, std::get<float>(value));
            }
            else if (std::holds_alternative<int>(value)){
                shader->setInt(name, std::get<int>(value));
            }
            else if (std::holds_alternative<glm::vec2>(value)){
                shader->setVec2(name, std::get<glm::vec2>(value));
            }
            else if (std::holds_alternative<glm::vec3>(value)){
                shader->setVec3(name, std::get<glm::vec3>(value));
            }
            else if (std::holds_alternative<glm::vec4>(value)){
                shader->setVec4(name, std::get<glm::vec4>(value));
            }
            else if (std::holds_alternative<glm::mat4>(value))
                shader->setMat4(name, std::get<glm::mat4>(value));
            else if (std::holds_alternative<Texture*>(value))
            {
                Texture* val = std::get<Texture*>(value);
                if (val != nullptr) {
                    val->Bind(textureUnit); // Bind to GL_TEXTURE0 + textureUnit
                    shader->setInt(name, textureUnit);
                    textureUnit++;
                }
            }
        }
    }

    void Material::StopUsing()
    {
        int textureUnit = 0;

        for (const auto& [name, value] : parameters)
        {
            std::visit([&](auto&& val) {
                using T = std::decay_t<decltype(val)>;

                if constexpr (std::is_same_v<T, Texture*>)
                {
                    if (val) {
                        val->UnBind(textureUnit);
                        textureUnit++;
                    }
                }
            }, value);
        }

        shader->Deactivate();
    }
}
