#include "level_serializer.hpp"

#include <nlohmann/json.hpp>

#include "engine/ecs/objects/actors/actor.hpp"

#include "engine/core/resources/resources_manager.hpp"

using namespace nlohmann;

namespace SHAME::Engine::Serialization{
    
    void LoadModelComponent(json &component, std::shared_ptr<ECS::Objects::Actor> a, json data){
        std::string mesh_name = component["mesh"];
        if (data["meshes"].contains(mesh_name)) {
            const std::string& mesh_path = data["meshes"][mesh_name];
            std::shared_ptr<Rendering::Mesh> mesh = Core::Resources::ResourcesManager::GetMesh(mesh_path);
            if(mesh)
                a->AddComponent<ECS::Components::Model>()->SetMesh(mesh);
            else
                throw std::runtime_error("[ERROR] [ENGINE/SERIALIZATION/LEVEL_IMPORTER] : Failed to retrieve mesh : "+mesh_path);
        }

        const auto& localMaterials = component["materials"];
        const auto& globalMaterials = data["materials"];

        int maxSlot = -1;
        for (auto it = localMaterials.begin(); it != localMaterials.end(); ++it) {
            int slot = std::stoi(it.key());
            if (slot > maxSlot) maxSlot = slot;
        }

        std::vector<std::shared_ptr<Rendering::Material>> materials(maxSlot + 1, nullptr);

        for (auto it = localMaterials.begin(); it != localMaterials.end(); ++it) {
            int slot = std::stoi(it.key());
            const std::string& materialName = it.value();

            auto matIt = globalMaterials.find(materialName);
            std::string materialPath;

            if (matIt != globalMaterials.end()) {
                materialPath = matIt.value();
            } else {
                throw std::runtime_error("[WARNING] [ENGINE/SERIALIZATION/LEVEL_IMPORTER] : Material '" + materialName
                        + "' not found in global materials. Using fallback.");
                materialPath = "engine/materials/unlit";
            }

            auto material = Core::Resources::ResourcesManager::GetMaterial(materialPath);
            if (!material) {
                throw std::runtime_error("[ERROR] [ENGINE/SERIALIZATION/LEVEL_IMPORTER] : Failed to load material at path: " + materialPath
                        + ". Using fallback.");
                material = Core::Resources::ResourcesManager::GetMaterial("engine/materials/unlit");

                if (!material) {
                    throw std::runtime_error("[ERROR] [ENGINE/SERIALIZATION/LEVEL_IMPORTER] : Failed to load fallback material: engine/materials/unlit");
                }
            }

            materials[slot] = material;
        }
        
        auto model = a->GetComponent<ECS::Components::Model>();
        model->SetMaterials(std::move(materials));

        if(component["active"])
            model->Activate();
        else
            model->DeActivate();
    }

    void LoadLightComponent(json &component, std::shared_ptr<ECS::Objects::Actor> a){
        a->AddComponent<ECS::Components::Light>();
        auto light = a->GetComponent<ECS::Components::Light>();

        if(component["light_type"] == "directionnal"){
            light->SetType(Rendering::LightType::Directional);
        }
        else if(component["light_type"] == "point"){
            light->SetType(Rendering::LightType::Point);
        }
        else if(component["light_type"] == "spot"){
            light->SetType(Rendering::LightType::Spot);
        }
        else{
            throw std::runtime_error("[ERROR] [ENGINE/SERIALIZATION/LEVEL_IMPORTER] : Light type not recognized : " + (std::string)component["light_type"]);
        }
        
        light->SetIntensity(component["intensity"]);
        light->SetRadius(component["radius"]);
        light->SetColor(COL_RGB(component["color"]["r"], component["color"]["g"], component["color"]["b"]));
        light->SetInnerCuttof(component["innerCutoff"]);
        light->SetOuterCutoff(component["outerCutoff"]);
        light->SetCastShadow(component["castShadow"]);
        
        if(component["active"])
            light->Activate();
        else
            light->DeActivate();
    }

    void LoadPhysicsBodyComponent(json &component, std::shared_ptr<ECS::Objects::Actor> a){
        a->AddComponent<ECS::Components::PhysicsBody>();

        auto physics_body = a->GetComponent<ECS::Components::PhysicsBody>();

        Physics::PhysicsShape shape;

        if(component["shape"] == "box"){
            shape = Physics::PhysicsShape::BOX;
        }
        else if(component["shape"] == "sphere"){
            shape = Physics::PhysicsShape::SPHERE;
        }
        else if(component["shape"] == "capsule"){
            shape = Physics::PhysicsShape::CAPSULE;
        }
        else if(component["shape"] == "cylinder"){
            shape = Physics::PhysicsShape::CYLINDER;
        }
        else{
            throw std::runtime_error("[ERROR] [ENGINE/SERIALIZATION/LEVEL_IMPORTER] : Physics shape not recognized : " + (std::string)component["shape"]);
        }
        
        JPH::EMotionType motion;

        if(component["motion_type"] == "dynamic"){
            motion = JPH::EMotionType::Dynamic;
        }
        else if(component["motion_type"] == "kinematic"){
            motion = JPH::EMotionType::Kinematic;
        }
        else if(component["motion_type"] == "static"){
            motion = JPH::EMotionType::Static;
        }
        else{
            throw std::runtime_error("[ERROR] [ENGINE/SERIALIZATION/LEVEL_IMPORTER] : Physics motion type not recognized : " + (std::string)component["shape"]);
        }

        physics_body->CreateBody(shape, glm::vec3(component["size"]["x"], component["size"]["y"], component["size"]["z"]), motion);

        if(component["active"])
            physics_body->Activate();
        else
            physics_body->DeActivate();
    }

    void LoadCameraComponent(json &component, std::shared_ptr<ECS::Objects::Actor> a){
        a->AddComponent<ECS::Components::Camera>();

        auto cam = a->GetComponent<ECS::Components::Camera>();

        int width = Rendering::Renderer::GetCurrentWidth();
        int height = Rendering::Renderer::GetCurrentHeight();

        float near = component["near"];
        float far = component["far"];

        cam->Init(width, height, near, far);
        
        if(component["active"])
            cam->Activate();
        else
            cam->DeActivate();
    }

    void LoadComponents(std::shared_ptr<ECS::Objects::Actor> a, json data, json actor){
        for(auto& component : actor["components"]){
            
            if (!component.contains("type")) continue;

            if(component["type"] == "transform"){
                a->transform->SetPosition(glm::vec3(component["position"]["x"],component["position"]["y"],component["position"]["z"]));
                a->transform->SetRotation(glm::vec3(component["rotation"]["x"],component["rotation"]["y"],component["rotation"]["z"]));
                a->transform->SetScale(glm::vec3(component["scale"]["x"],component["scale"]["y"],component["scale"]["z"]));
            }
            else if(component["type"] == "model"){
                LoadModelComponent(component, a, data);
            }
            else if(component["type"] == "light"){
                LoadLightComponent(component, a);
            }
            else if(component["type"] == "physics_body"){
                LoadPhysicsBodyComponent(component, a);
            }
            else if(component["type"] == "camera"){
                LoadCameraComponent(component, a);
            }
        }
    }

    void LoadActor(std::shared_ptr<ECS::Objects::Actor> a, json data, json actor){

        if (actor.contains("children") && actor["children"].is_array() && !actor["children"].empty()) {
            for (auto& child : actor["children"]) {
                std::shared_ptr<ECS::Objects::Actor> b = std::make_shared<ECS::Objects::Actor>(child["name"]);
                a->AddChild(b);
                LoadActor(b, data, child);
            }
        }

        LoadComponents(a, data, actor);
    }

    std::shared_ptr<Levels::Level> LevelSerializer::ImportLevel(const Filesystem::Path path)
    {
        std::string src = path.ReadFile();

        try {
            json data = json::parse(src);

            std::shared_ptr<Levels::Level> l = std::make_shared<Levels::Level>(data["name"]);

            for(auto& actor : data["actors"])
            {
                std::shared_ptr<ECS::Objects::Actor> a = std::make_shared<ECS::Objects::Actor>(actor["name"]);
                l->AddActor(a);
                LoadActor(a, data, actor);
            }

            return l;

        } catch (const json::parse_error& e) {
            throw std::runtime_error("[ERROR] [ENGINE/SERIALIZATION/LEVEL_IMPORTER] : JSON parse error: " + (std::string)e.what());
        }
    }

}