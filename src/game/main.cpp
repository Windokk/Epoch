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

    std::shared_ptr<Rendering::Material> mat = std::make_shared<Rendering::Material>(std::make_shared<Rendering::Shader>("engine_resources/shaders/mesh/default.vert", "engine_resources/shaders/mesh/default.frag"));

    Rendering::Texture* texture = new Rendering::Texture(Filesystem::Path("engine_resources/textures/white.png"));

    mat->SetParameter("texture1", texture);
    mat->SetParameter("useTexture", 1);

    Actor* actor1 = new Actor("obj1");
    Levels::LevelManager::GetLevelAt(0)->AddActor(actor1);
    actor1->AddComponent<ModelComponent>();
    std::shared_ptr<Rendering::Mesh> mesh = std::make_shared<Rendering::Mesh>(Filesystem::Path("engine_resources/models/sphere.obj"));
    actor1->GetComponent<ModelComponent>().SetMaterial(mat);
    actor1->GetComponent<ModelComponent>().SetMesh(mesh);
    actor1->AddComponent<PhysicsComponent>()->CreateBody(Physics::SPHERE, glm::vec3(1,1,1), JPH::EMotionType::Dynamic);


    Actor* actor2 = new Actor("obj2");
    Levels::LevelManager::GetLevelAt(0)->AddActor(actor2);
    actor2->transform->SetPosition(glm::vec3(0, -10, 0));
    actor2->transform->Scale(glm::vec3(10,1,10));
    actor2->AddComponent<PhysicsComponent>()->CreateBody(Physics::BOX, glm::vec3(10, 0.02f, 10), JPH::EMotionType::Static);
    actor2->AddComponent<ModelComponent>()->SetMesh(std::make_shared<Rendering::Mesh>(Filesystem::Path("engine_resources/models/plane.obj")));
    actor2->GetComponent<ModelComponent>().SetMaterial(mat);
    actor2->AddComponent<AudioSource>()->SetPath("engine_resources/sounds/TownTheme.mp3");
    actor2->GetComponent<AudioSource>().SetVolume(10.0f);

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
    actor5->GetComponent<Light>().SetColor(COL_RGB(1, 0, 0));

    Rendering::Renderer::AddRenderPass(Rendering::RenderStage::Scene, RenderPassMain, std::make_shared<Rendering::FrameBuffer>(sceneFB), true, Rendering::BlendMode::Normal);

    while (!engine.shouldEnd())
    {
        engine.Run();

        if(glfwGetKey(engine.GetGLFWWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            break;
        }
    }

    engine.Destroy();

    return 0;

}