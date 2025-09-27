#include "engine/core/engine.hpp"
#include "engine/serialization/level/level_serializer.hpp"
#include "engine/core/resources/resources_manager.hpp"
#include "module_loader.hpp"

using namespace EPOCH::Engine;
using namespace EPOCH::Engine::Rendering;
using namespace EPOCH::Engine::Input;
using namespace EPOCH::Engine::ECS::Components;
using namespace EPOCH::Engine::ECS::Objects;
using namespace EPOCH::Launcher;

void RenderPassMain() {
    Rendering::Renderer::GetInstance().DrawScene();
}

Debugging::Level minDebugLevel = Debugging::Level::Log;

Core::EngineCreationSettings ComputeEngineSettings(int argc, char* argv[]) {
    Core::EngineCreationSettings settings;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--width") == 0 && i + 1 < argc) {
            settings.windowWidth = std::stoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc) {
            settings.windowHeight = std::stoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--fullscreen") == 0) {
            settings.fullscreen = true;
        }
        else if (strcmp(argv[i], "--no-vsync") == 0) {
            settings.vsync = false;
        }
        else if (strcmp(argv[i], "--fps") == 0 && i + 1 < argc) {
            settings.targetFPS = std::stoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--root") == 0 && i + 1 < argc) {
            settings.rootPath = argv[++i];
        }
        else if (strcmp(argv[i], "--gravity") == 0 && i + 3 < argc) {
            float x = std::stof(argv[++i]);
            float y = std::stof(argv[++i]);
            float z = std::stof(argv[++i]);
            settings.gravity = glm::vec3(x, y, z);
        }
        else if (strcmp(argv[i], "--debug") == 0 && i + 1 < argc) {
            std::string level = argv[++i];
            if (level == "log") {
                minDebugLevel = Debugging::Level::Log;
            } else if (level == "info") {
                minDebugLevel = Debugging::Level::Info;
            } else if (level == "warning") {
                minDebugLevel = Debugging::Level::Warning;
            } else if (level == "error") {
                minDebugLevel = Debugging::Level::Error;
            } else if (level == "fatal") {
                minDebugLevel = Debugging::Level::Fatal;
            } else {
                std::cerr << "Unknown debug level: " << level << ". Using default (Log).\n";
            }
        }
    }

    return settings;
}

int main(int argc, char *argv[]) {

    Core::EngineCreationSettings engineSettings = ComputeEngineSettings(argc, argv);

    if (!ModuleLoader::GetInstance().LoadGameModule(
#if defined(_WIN32)
        "libGameModule.dll"
#else
        "./libGameModule.so"
#endif
    , minDebugLevel)) {
        return -1;
    }

    Core::EngineInstance engine{engineSettings};

    std::shared_ptr<Rendering::Shader> fbShader = Core::Resources::ResourcesManager::GetInstance().GetShader("shaders\\fb\\framebuffer");
    Rendering::FrameBuffer sceneFB = {static_cast<float>(Rendering::Renderer::GetInstance().GetCurrentWidth()), static_cast<float>(Rendering::Renderer::GetInstance().GetCurrentHeight()), fbShader, true};
    
    Rendering::Renderer::GetInstance().AddRenderPass(Rendering::RenderStage::Scene, RenderPassMain, std::make_shared<Rendering::FrameBuffer>(sceneFB), true, Rendering::BlendMode::Normal);

    std::shared_ptr<Levels::Level> l = Core::Resources::ResourcesManager::GetInstance().GetLevel("sponza.lvl");

    Levels::LevelManager::GetInstance().LoadLevel(l);

    while (!engine.shouldEnd())
    {
        if(!engine.Run()){
            break;
        };
    }

    engine.Destroy();
    std::cout << "EPOCH Engine has finished. Press Enter to exit..." << std::endl;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();

    return 0;

}