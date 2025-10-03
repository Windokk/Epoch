#include "material_serializer.hpp"

#include "engine/core/resources/resources_manager.hpp"

#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace Epoch::Engine::Serialization{

    using namespace Core::Resources;
    using namespace Rendering;

    std::shared_ptr<Material> MaterialSerializer::ImportMaterial(Filesystem::Path path)
    {
        std::string src = path.ReadFile();

        try {
            json data = json::parse(src);

            std::shared_ptr<Material> mat = std::make_shared<Material>();

            std::shared_ptr<Shader> shader = ResourcesManager::GetInstance().GetShader(data["shader"]);

            if(!shader){
                DEBUG_ERROR("No shader found : " + ((std::string)data["shader"]));
                return nullptr;
            }

            mat->Init(shader, data["recievesShadows"]);

            for(auto& uniform : data["uniforms"]){
                for (auto it = uniform.begin(); it != uniform.end(); ++it) {
                    const std::string& name = it.key();
                    const auto& value = it.value();

                    // Texture
                    if (value.is_string()) {
                        std::shared_ptr<Texture> tex = ResourcesManager::GetInstance().GetTexture(value.get<std::string>());
                        if(tex){
                            mat->SetParameter(name, tex);
                        }
                        else{
                            DEBUG_ERROR("No texture found : " + value.get<std::string>());
                            return nullptr;
                        }
                    }
                    else if(value.is_boolean()){
                        mat->SetParameter(name, value.get<bool>());
                    }
                    // Float
                    else if (value.is_number_float()) {
                        mat->SetParameter(name, value.get<float>());
                    }
                    // Int
                    else if (value.is_number_integer()) {
                        mat->SetParameter(name, value.get<int>());
                    }
                    // Vectors
                    else if (value.is_array()) {
                        size_t len = value.size();
                        if (len == 2) {
                            mat->SetParameter(name, glm::vec2(value[0].get<float>(), value[1].get<float>()));
                        }
                        else if (len == 3) {
                            mat->SetParameter(name, glm::vec3(value[0].get<float>(), value[1].get<float>(), value[2].get<float>()));
                        }
                        else if (len == 4) {
                            mat->SetParameter(name, glm::vec4(value[0].get<float>(), value[1].get<float>(), value[2].get<float>(), value[3].get<float>()));
                        }
                        else if (len == 16) {
                            glm::mat4 mat4;
                            for (int i = 0; i < 16; ++i)
                                mat4[i / 4][i % 4] = value[i].get<float>();
                            mat->SetParameter(name, mat4);
                        } else {
                            DEBUG_ERROR("Unknown array size for uniform: " + name);
                        }
                    }
                    else {
                        DEBUG_ERROR("Unsupported uniform value type for: " + name);
                    }
                }

                
            }

            return mat;

        } catch (const json::parse_error& e) {
            DEBUG_ERROR("JSON parse error: " + (std::string)e.what());
            return nullptr;
        }
    }
}

