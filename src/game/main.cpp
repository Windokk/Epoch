#include "engine/core/engine.hpp"

using namespace SHAME::Engine;
using namespace Rendering;
using namespace Input;
using namespace ECS::Components;
using namespace ECS::Objects;

void RenderPassMain() {
    Rendering::Renderer::DrawScene();
}


int main(int argc, char *argv[]) {

    Core::EngineInstance engine{};

    Levels::Level l{"Test Scene"};

    Levels::LevelManager::GetInstance().LoadLevel(std::make_unique<Levels::Level>(std::move(l)));

    std::shared_ptr<Rendering::Shader> fbShader = std::make_shared<Rendering::Shader>("engine_resources/shaders/fb/framebuffer.vert", "engine_resources/shaders/fb/framebuffer.frag");
    Rendering::FrameBuffer sceneFB = {static_cast<float>(Rendering::Renderer::GetCurrentWidth()), static_cast<float>(Rendering::Renderer::GetCurrentHeight()), fbShader};

    std::shared_ptr<Rendering::Shader> litShader = std::make_shared<Rendering::Shader>("engine_resources/shaders/mesh/default.vert", "engine_resources/shaders/mesh/lit.frag");

    std::shared_ptr<Rendering::Material> mat = std::make_shared<Rendering::Material>(litShader, true);

    Rendering::Texture* texture = new Rendering::Texture(Filesystem::Path("engine_resources/textures/white.png"));

    mat->SetParameter("diffuse", texture);
    
    Actor* actor1 = new Actor("obj1");
    Levels::LevelManager::GetLevelAt(0)->AddActor(actor1);
    actor1->AddComponent<Model>();
    std::shared_ptr<Rendering::Mesh> mesh = std::make_shared<Rendering::Mesh>(Filesystem::Path("engine_resources/models/sphere.fbx"));
    actor1->GetComponent<Model>().SetMaterial(mat);
    actor1->GetComponent<Model>().SetMesh(mesh);
    actor1->AddComponent<PhysicsBody>()->CreateBody(Physics::SPHERE, glm::vec3(1,1,1), JPH::EMotionType::Dynamic);

    Actor* actor2 = new Actor("obj2");
    Levels::LevelManager::GetLevelAt(0)->AddActor(actor2);
    actor2->transform->SetPosition(glm::vec3(0, -10, 0));
    actor2->transform->SetRotation(glm::vec3(0, 0, 10));
    actor2->transform->SetScale(glm::vec3(10,10,10));
    actor2->AddComponent<PhysicsBody>()->CreateBody(Physics::BOX, glm::vec3(10, 0.02f, 10), JPH::EMotionType::Static);
    actor2->AddComponent<Model>()->SetMesh(std::make_shared<Rendering::Mesh>(Filesystem::Path("engine_resources/models/plane.fbx")));
    actor2->GetComponent<Model>().SetMaterial(mat);
    actor2->AddComponent<AudioSource>()->SetPath("engine_resources/sounds/TownTheme.mp3");
    actor2->GetComponent<AudioSource>().SetVolume(10.0f);

    Actor* rubikscube = new Actor("rubik's cube");
    Levels::LevelManager::GetLevelAt(0)->AddActor(rubikscube);
    actor2->transform->SetPosition(glm::vec3(2, 0, 0));
    //actor2->AddComponent<Model>()->SetMeshAndMaterialFromPath(new Filesystem::Path("engine_resources/models/rubik's_cube.fbx"));

    Actor* actor3 = new Actor("obj3");
    Levels::LevelManager::GetLevelAt(0)->AddActor(actor3);
    actor3->transform->SetPosition(glm::vec3(0, 10, 0));
    actor3->transform->SetRotation(glm::vec3(90, 0, 0));
    actor3->AddComponent<Light>()->SetIntensity(0.1f);

    Actor* actor4 = new Actor("obj4");
    Levels::LevelManager::GetLevelAt(0)->AddActor(actor4);
    actor4->transform->SetPosition(glm::vec3(2, 10, 2));
    actor4->transform->SetRotation(glm::vec3(90, 0, 0));
    actor4->AddComponent<Light>()->SetType(LightType::Spot);

    Actor* actor5 = new Actor("obj5");
    Levels::LevelManager::GetLevelAt(0)->AddActor(actor5);
    actor5->transform->SetPosition(glm::vec3(2, 10, 2));
    actor5->AddComponent<Light>()->SetType(LightType::Point);

    Actor* actor6 = new Actor("camera");
    Levels::LevelManager::GetLevelAt(0)->AddActor(actor6);
    actor6->AddComponent<Camera>();
    actor6->transform->SetPosition(glm::vec3(0,0,-40));

    Rendering::Renderer::AddRenderPass(Rendering::RenderStage::Scene, RenderPassMain, std::make_shared<Rendering::FrameBuffer>(sceneFB), true, Rendering::BlendMode::Normal);

    while (!engine.shouldEnd())
    {
        engine.Run();

        if(InputManager::WasKeyPressed(KEY_ESCAPE))
        {
            break;
        }
    }

    engine.Destroy();

    return 0;

}