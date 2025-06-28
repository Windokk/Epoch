#include "engine/core/engine.hpp"

#include <iostream>

using namespace SHAME::Engine;
using namespace ECS::Components;

void RenderPassMain() {
    Rendering::Renderer::DrawScene();

}

int main(int argc, char *argv[]) {

    Core::EngineInstance engine{};

    Levels::Level l{"Test Scene"};

    Levels::LevelManager& levelManager = Levels::LevelManager::GetInstance();
    levelManager.LoadLevel(std::make_unique<Levels::Level>(std::move(l)));

    std::shared_ptr<Rendering::Shader> fbShader = std::make_shared<Rendering::Shader>("resources/shaders/fb/framebuffer.vert", "resources/shaders/fb/framebuffer.frag");
    Rendering::FrameBuffer sceneFB = {static_cast<float>(Rendering::Renderer::GetWidth()), static_cast<float>(Rendering::Renderer::GetHeight()), fbShader};

    std::shared_ptr<Rendering::Material> mat = std::make_shared<Rendering::Material>(std::make_shared<Rendering::Shader>("resources/shaders/mesh/default.vert", "resources/shaders/mesh/default.frag"));

    Rendering::Texture* texture = new Rendering::Texture(Filesystem::Path("resources/textures/white.png"));

    mat->SetParameter("texture1", texture);
    mat->SetParameter("useTexture", 1);

    ECS::Objects::Actor* actor1 = new ECS::Objects::Actor("obj1");
    Levels::LevelManager::GetLevelAt(0)->AddActor(actor1);
    actor1->AddComponent<ModelComponent>();
    std::shared_ptr<Rendering::Mesh> mesh = std::make_shared<Rendering::Mesh>(Filesystem::Path("resources/models/sphere.obj"));
    actor1->GetComponent<ModelComponent>().SetMaterial(mat);
    actor1->GetComponent<ModelComponent>().SetMesh(mesh);
    actor1->AddComponent<PhysicsComponent>()->CreateBody(Physics::SPHERE, glm::vec3(1,1,1), JPH::EMotionType::Dynamic);

    ECS::Objects::Actor* actor2 = new ECS::Objects::Actor("obj2");
    Levels::LevelManager::GetLevelAt(0)->AddActor(actor2);
    actor2->AddComponent<Light>();

    ECS::Objects::Actor* actor3 = new ECS::Objects::Actor("obj3");
    Levels::LevelManager::GetLevelAt(0)->AddActor(actor3);
    actor3->transform->SetPosition(glm::vec3(0, -10, 0));
    actor3->AddComponent<PhysicsComponent>()->CreateBody(Physics::BOX, glm::vec3(1, 0.02f, 1), JPH::EMotionType::Static);
    actor3->AddComponent<ModelComponent>()->SetMesh(std::make_shared<Rendering::Mesh>(Filesystem::Path("resources/models/plane.obj")));
    actor3->GetComponent<ModelComponent>().SetMaterial(mat);
    actor3->AddComponent<AudioSource>()->SetPath("resources/sounds/TownTheme.mp3");
    actor3->GetComponent<AudioSource>().SetVolume(10.0f);

    Rendering::Renderer::AddRenderPass(Rendering::RenderStage::Scene, RenderPassMain, std::make_shared<Rendering::FrameBuffer>(sceneFB), true, Rendering::BlendMode::Normal);

    while (!engine.shouldEnd())
    {
        engine.Run();

        if (glfwGetKey(engine.GetGLFWWindow(), GLFW_KEY_P) == GLFW_PRESS){
            actor3->GetComponent<AudioSource>().Play();
        }
    }

    engine.Destroy();

    return 0;

}