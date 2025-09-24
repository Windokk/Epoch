#pragma once

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #define NOCRYPT
    #define NORPC
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

#include "engine/debugging/debugger.hpp"

#include "engine/ecs/components/core/registry/component_registry.hpp"
#include "engine/inputs/input_manager.hpp"

namespace EPOCH::Launcher{
    class ModuleLoader{

        public:

            static ModuleLoader& GetInstance() {
                static ModuleLoader instance;
                return instance;
            }

            bool LoadGameModule(const std::string& path, const EPOCH::Engine::Debugging::Level minDebugLevel) {
                #if defined(_WIN32)
                    HMODULE hLib = LoadLibraryA(path.c_str());
                    if (!hLib) {
                        DEBUG_FATAL("Failed to load game module : " + path);
                    }
                    
                    using InitFunc = void(*)(EPOCH::Engine::Debugging::Debugger*, EPOCH::Engine::Debugging::Level, EPOCH::Engine::ECS::Components::ComponentRegistry*, EPOCH::Engine::Input::InputManager*);
                    InitFunc init = reinterpret_cast<InitFunc>(
                        GetProcAddress(hLib, "InitializeSingletons"));

                    if (!init) {
                        DEBUG_FATAL("Failed to find InitializeSingletons() in .dll");
                        
                    }
                
                    init(&EPOCH::Engine::Debugging::Debugger::GetInstance(), minDebugLevel, &EPOCH::Engine::ECS::Components::gSharedComponentRegistry, &EPOCH::Engine::Input::gSharedInputManager);

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

                    using InitFunc = void(*)(EPOCH::Engine::ECS::Components::ComponentRegistry*, EPOCH::Engine::Input::InputManager*);
                    InitFunc init = reinterpret_cast<InitFunc>(dlsym(handle, "InitializeSingletons"));
                    if (!init) {
                        DEBUG_FATAL("Failed to find InitializeSingletons() in .so");
                    }

                    init(&EPOCH::Engine::ECS::Components::gSharedComponentRegistry, &EPOCH::Engine::Inputs::gSharedInputManager);

                #endif

                    DEBUG_LOG("Game module loaded : " + path);
                    return true;
            }
        private:

            ModuleLoader() = default;
            ~ModuleLoader() = default;
            ModuleLoader(const ModuleLoader&) = delete;
            ModuleLoader& operator=(const ModuleLoader&) = delete;
    };
}