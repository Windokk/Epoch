#include "engine/core/engine.hpp"
#include "engine/serialization/level/level_serializer.hpp"
#include "engine/core/resources/resources_manager.hpp"
#include "module_loader.hpp"
#include "engine/rendering/ui/text.hpp"

using namespace EPOCH::Engine;
using namespace EPOCH::Engine::Rendering;
using namespace EPOCH::Engine::Input;
using namespace EPOCH::Engine::ECS::Components;
using namespace EPOCH::Engine::ECS::Objects;
using namespace EPOCH::Launcher;


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
    
    auto RenderPassMain = [] { Rendering::Renderer::GetInstance().DrawScene(); };

    
    Rendering::UI::Font *font = new Rendering::UI::Font("project_resources\\fonts\\Roboto-Medium.ttf", 30);
    ECS::Objects::Actor a = ECS::Objects::Actor("test");
    a.transform->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
    a.transform->SetRotation(glm::vec3(0, 90, 0));
    std::string neuil = "FPS = 60.0";
    Rendering::UI::Text *fpsText = new Rendering::UI::Text(*font, neuil, COL_RGBA(1,1,1,1), *a.transform);
    std::shared_ptr<Rendering::Shader> textShader = Core::Resources::ResourcesManager::GetInstance().GetShader("shaders\\text\\text");
    Rendering::FrameBuffer uiFB = {static_cast<float>(Rendering::Renderer::GetInstance().GetCurrentWidth()), static_cast<float>(Rendering::Renderer::GetInstance().GetCurrentHeight()), fbShader, true};
    

    auto RenderPassUI = [fpsText, textShader] {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        fpsText->SetText("FPS = "+std::to_string(static_cast<float>(1/Time::TimeManager::GetInstance().GetDeltaTime())));
        fpsText->Draw(*textShader, CameraManager::GetInstance().GetActiveCamera()->GetProjection(), CameraManager::GetInstance().GetActiveCamera()->GetView());
        glDisable(GL_BLEND);
    };

    Rendering::Renderer::GetInstance().AddRenderPass(Rendering::RenderStage::Scene, RenderPassMain, std::make_shared<Rendering::FrameBuffer>(sceneFB), true, Rendering::BlendMode::Normal);
    Rendering::Renderer::GetInstance().AddRenderPass(Rendering::RenderStage::UI, RenderPassUI, std::make_shared<FrameBuffer>(uiFB));

    std::shared_ptr<Levels::Level> l = Core::Resources::ResourcesManager::GetInstance().GetLevel("sponza.lvl");
    l->AddActor(std::make_shared<Actor>(a));
    Levels::LevelManager::GetInstance().LoadLevel(l);


    while (!engine.shouldEnd())
    {
        if(!engine.Run()){
            break;
        };
    }

    font->Cleanup();
    engine.Destroy();
    std::cout << "EPOCH Engine has finished. Press Enter to exit..." << std::endl;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();

    return 0;

}