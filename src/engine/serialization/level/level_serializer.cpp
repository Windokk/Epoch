#include "level_serializer.hpp"

#include <nlohmann/json.hpp>

#include "engine/ecs/objects/actors/actor.hpp"

using namespace nlohmann;

namespace SHAME::Engine::Serialization{

    std::shared_ptr<Levels::Level> LevelSerializer::ImportLevel(const Filesystem::Path path)
    {
        std::string src = path.ReadFile();

        try {
            json data = json::parse(src);

            std::shared_ptr<Levels::Level> l = std::make_shared<Levels::Level>(data["name"]);

            for(auto& actor : data["actors"])
            {
                std::shared_ptr<ECS::Objects::Actor> a = std::make_shared<ECS::Objects::Actor>(actor["name"]);
                for(auto& component : actor["components"]){
                    if(component["type"] == "transform"){
                        a->transform->SetPosition(glm::vec3(component["position"]["x"],component["position"]["y"],component["position"]["z"]));
                        a->transform->SetRotation(glm::vec3(component["rotation"]["x"],component["rotation"]["y"],component["rotation"]["z"]));
                        a->transform->SetScale(glm::vec3(component["scale"]["x"],component["scale"]["y"],component["scale"]["z"]));
                    }
                    else if(component["type"] == "model"){
                        
                    }
                    else if(component["type"] == "light"){
                        
                    }
                    else if(component["type"] == "physics_body"){

                    } 
                }
            }

            return l;

        } catch (const json::parse_error& e) {
            throw std::runtime_error("[ERROR] [ENGINE/SERIALIZATION/LEVEL_IMPORTER] : JSON parse error: " + (std::string)e.what());
        }
    }
}