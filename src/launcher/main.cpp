#include "engine/core/engine.hpp"
#include "engine/serialization/level/level_serializer.hpp"
#include "engine/core/resources/resources_manager.hpp"
#include "engine/ecs/components/core/component_registry.hpp"

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #define NOCRYPT
    #define NORPC
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

using namespace SHAME::Engine;
using namespace SHAME::Engine::Rendering;
using namespace SHAME::Engine::Input;
using namespace SHAME::Engine::ECS::Components;
using namespace SHAME::Engine::ECS::Objects;

void RenderPassMain() {
    Rendering::Renderer::DrawScene();
}

bool LoadGameModule(const std::string& path) {
#if defined(_WIN32)
    HMODULE hLib = LoadLibraryA(path.c_str());
    if (!hLib) {
        throw std::runtime_error("[ERROR] [LAUNCHER/MAIN] : Failed to load game module : " + path);
    }
    
    using InitRegistryFunc = void(*)(SHAME::Engine::ECS::Components::ComponentRegistry*);
    InitRegistryFunc init = reinterpret_cast<InitRegistryFunc>(
        GetProcAddress(hLib, "InitializeComponentRegistry"));

    if (!init) {
        throw std::runtime_error("[ERROR] [LAUNCHER/MAIN] : Failed to find InitializeComponentRegistry() in .dll");
        
    }
 
    init(&SHAME::Engine::ECS::Components::gSharedComponentRegistry);

    using RegisterFunc = void(*)();
    RegisterFunc registerComponents = reinterpret_cast<RegisterFunc>(
        GetProcAddress(hLib, "RegisterGameComponents"));

    if (!registerComponents) {
        throw std::runtime_error("[ERROR] [LAUNCHER/MAIN] : Could not find RegisterGameComponents() in GameModule");
    }

    registerComponents();

#else
    void* handle = dlopen(path.c_str(), RTLD_NOW);
    if (!handle) {
        throw std::runtime_error("[ERROR] [LAUNCHER/MAIN] : Failed to load game module : " + dlerror());
    }

    using InitRegistryFunc = void(*)(SHAME::Engine::ECS::Components::ComponentRegistry*);
    InitRegistryFunc init = reinterpret_cast<InitRegistryFunc>(dlsym(handle, "InitializeComponentRegistry"));
    if (!init) {
        throw std::runtime_error("[ERROR] Failed to find InitializeComponentRegistry() in .so");
    }

    init(&SHAME::Engine::ECS::Components::gSharedComponentRegistry);

#endif

    std::cout << "[INFO] [LAUNCHER/MAIN] : Game module loaded : " << path << "\n";
    return true;
}


int main(int argc, char *argv[]) {

    if (!LoadGameModule(
#if defined(_WIN32)
        "libGameModule.dll"
#else
        "./libGameModule.so"
#endif
    )) {
        return -1;
    }

    Core::EngineInstance engine{};

    std::shared_ptr<Rendering::Shader> fbShader = std::make_shared<Rendering::Shader>(Filesystem::Path("engine_resources/shaders/fb/framebuffer.vert"), Filesystem::Path("engine_resources/shaders/fb/framebuffer.frag"));
    Rendering::FrameBuffer sceneFB = {static_cast<float>(Rendering::Renderer::GetCurrentWidth()), static_cast<float>(Rendering::Renderer::GetCurrentHeight()), fbShader};
    
    Core::Resources::ResourcesManager::LoadResources(Filesystem::Path("project_resources"));

    Rendering::Renderer::AddRenderPass(Rendering::RenderStage::Scene, RenderPassMain, std::make_shared<Rendering::FrameBuffer>(sceneFB), true, Rendering::BlendMode::Normal);

    std::shared_ptr<Levels::Level> l = Core::Resources::ResourcesManager::GetLevel("level1.lvl");

    Levels::LevelManager::LoadLevel(l);

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