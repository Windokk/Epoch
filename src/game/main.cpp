#include "engine/core/engine.hpp"
#include "engine/serialization/level/level_serializer.hpp"
#include "engine/core/resources/resources_manager.hpp"

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

    std::shared_ptr<Rendering::Shader> fbShader = std::make_shared<Rendering::Shader>(Filesystem::Path("engine_resources/shaders/fb/framebuffer.vert"), Filesystem::Path("engine_resources/shaders/fb/framebuffer.frag"));
    Rendering::FrameBuffer sceneFB = {static_cast<float>(Rendering::Renderer::GetCurrentWidth()), static_cast<float>(Rendering::Renderer::GetCurrentHeight()), fbShader};
    
    //std::shared_ptr<Levels::Level> neuil = Serialization::LevelSerializer::ImportLevel(Filesystem::Path("project_resources/level1.json"));

    Core::Resources::ResourcesManager::LoadResources(Filesystem::Path("project_resources"));

    Rendering::Renderer::AddRenderPass(Rendering::RenderStage::Scene, RenderPassMain, std::make_shared<Rendering::FrameBuffer>(sceneFB), true, Rendering::BlendMode::Normal);

    while (!engine.shouldEnd())
    {
        //engine.Run();

        if(InputManager::WasKeyPressed(KEY_ESCAPE))
        {
            break;
        }
    }

    engine.Destroy();

    return 0;

}