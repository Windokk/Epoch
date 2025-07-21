#include "engine/core/engine.hpp"
#include "engine/serialization/level/level_serializer.hpp"
#include "engine/core/resources/resources_manager.hpp"
#include "engine/ecs/components/core/registry/component_registry.hpp"

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
        DEBUG_FATAL("Failed to load game module : " + path);
    }
    
    using InitFunc = void(*)(SHAME::Engine::ECS::Components::ComponentRegistry*, SHAME::Engine::Input::InputManager*);
    InitFunc init = reinterpret_cast<InitFunc>(
        GetProcAddress(hLib, "InitializeSingletons"));

    if (!init) {
        DEBUG_FATAL("Failed to find InitializeSingletons() in .dll");
        
    }
 
    init(&SHAME::Engine::ECS::Components::gSharedComponentRegistry, &SHAME::Engine::Input::gSharedInputManager);

    using RegisterFunc = void(*)();
    RegisterFunc registerComponents = reinterpret_cast<RegisterFunc>(
        GetProcAddress(hLib, "RegisterGameComponents"));

    if (!registerComponents) {
        DEBUG_FATAL("Could not find RegisterGameComponents() in GameModule");
    }

    registerComponents();

#else
    void* handle = dlopen(path.c_str(), RTLD_NOW);
    if (!handle) {
        DEBUG_FATAL("Failed to load game module : " + dlerror());
    }

    using InitFunc = void(*)(SHAME::Engine::ECS::Components::ComponentRegistry*, SHAME::Engine::Input::InputManager*);
    InitFunc init = reinterpret_cast<InitFunc>(dlsym(handle, "InitializeSingletons"));
    if (!init) {
        DEBUG_FATAL("Failed to find InitializeSingletons() in .so");
    }

    init(&SHAME::Engine::ECS::Components::gSharedComponentRegistry, &SHAME::Engine::Inputs::gSharedInputManager);

#endif

    DEBUG_LOG("Game module loaded : " + path);
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

    std::shared_ptr<Rendering::Shader> fbShader = Core::Resources::ResourcesManager::GetShader("shaders\\fb\\framebuffer");
    Rendering::FrameBuffer sceneFB = {static_cast<float>(Rendering::Renderer::GetCurrentWidth()), static_cast<float>(Rendering::Renderer::GetCurrentHeight()), fbShader};
    
    Rendering::Renderer::AddRenderPass(Rendering::RenderStage::Scene, RenderPassMain, std::make_shared<Rendering::FrameBuffer>(sceneFB), true, Rendering::BlendMode::Normal);

    std::shared_ptr<Levels::Level> l = Core::Resources::ResourcesManager::GetLevel("level1.lvl");

    Levels::LevelManager::LoadLevel(l);

    Debugging::Debugger::SetMinimumLevel(Debugging::Level::Warning);

    while (!engine.shouldEnd())
    {
        engine.Run();

        if(GetInputManager().WasKeyPressed(KEY_ESCAPE))
        {
            break;
        }
    }

    engine.Destroy();

    return 0;

}