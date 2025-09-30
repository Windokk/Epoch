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

            void LoadGameModule(const std::string& modulePath, const EPOCH::Engine::Debugging::Level minDebugLevel) {
                #if defined(_WIN32)
                    HMODULE hLib = LoadLibraryA(modulePath.c_str());
                    if (!hLib) {
                        DEBUG_FATAL("Failed to load game module : " + modulePath);
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
                    void* handle = dlopen(modulePath.c_str(), RTLD_NOW);
                    if (!handle) {
                        DEBUG_FATAL(std::string("Failed to load game module : ") + dlerror());
                    }

                    using InitFunc = void(*)(EPOCH::Engine::Debugging::Debugger*, EPOCH::Engine::Debugging::Level, EPOCH::Engine::ECS::Components::ComponentRegistry*, EPOCH::Engine::Input::InputManager*);
                    InitFunc init = reinterpret_cast<InitFunc>(dlsym(handle, "InitializeSingletons"));
                    if (!init) {
                        DEBUG_FATAL("Failed to find InitializeSingletons() in .so");
                    }

                    init(&EPOCH::Engine::Debugging::Debugger::GetInstance(), minDebugLevel, &EPOCH::Engine::ECS::Components::gSharedComponentRegistry, &EPOCH::Engine::Input::gSharedInputManager);

                    using RegisterFunc = void(*)();
                    RegisterFunc registerComponents = reinterpret_cast<RegisterFunc>(dlsym(handle, "RegisterGameComponents"));

                    if (!registerComponents) {
                        DEBUG_FATAL("Could not find RegisterGameComponents() in GameModule");
                    }

                    registerComponents();

                #endif

                    DEBUG_LOG("Game module loaded : " + modulePath);
            }
        
            void LoadEditorModule(const std::string& modulePath){
                #if defined(_WIN32)
                    HMODULE hLib = LoadLibraryA(modulePath.c_str());
                    if (!hLib) {
                        DEBUG_FATAL("Failed to load editor module : " + modulePath);
                    }
                    
                    using InitFunc = void(*)(EPOCH::Engine::Debugging::Debugger*, EPOCH::Engine::Rendering::Renderer*, EPOCH::Engine::Core::Resources::ResourcesManager*, EPOCH::Engine::Rendering::CameraManager*, EPOCH::Engine::Time::TimeManager*);
                    InitFunc init = reinterpret_cast<InitFunc>(
                        GetProcAddress(hLib, "InitializeSingletons"));

                    if (!init) {
                        DEBUG_FATAL("Failed to find InitializeSingletons() in .dll");
                        
                    }
                
                    init(&EPOCH::Engine::Debugging::Debugger::GetInstance(), &EPOCH::Engine::Rendering::Renderer::GetInstance(),
                            &EPOCH::Engine::Core::Resources::ResourcesManager::GetInstance(),
                            &EPOCH::Engine::Rendering::CameraManager::GetInstance(),
                            &EPOCH::Engine::Time::TimeManager::GetInstance());

                    using StartFunc = void(*)();
                    StartFunc start = reinterpret_cast<StartFunc>(
                        GetProcAddress(hLib, "EditorStart"));

                    if (!start) {
                        DEBUG_FATAL("Failed to find StartEditor() in .dll");
                    }

                    start();

                #else
                    void* handle = dlopen(modulePath.c_str(), RTLD_NOW);
                    if (!handle) {
                        DEBUG_FATAL(std::string("Failed to load editor module : ") + dlerror());
                    }

                    using InitFunc = void(*)(EPOCH::Engine::Debugging::Debugger* debugger, 
                                                            EPOCH::Engine::Rendering::Renderer* renderer, 
                                                            EPOCH::Engine::Core::Resources::ResourcesManager* resourcesManager, 
                                                            EPOCH::Engine::Rendering::CameraManager* cameraManager,
                                                            EPOCH::Engine::Time::TimeManager* timeManager);
                    InitFunc init = reinterpret_cast<InitFunc>(dlsym(handle, "InitializeSingletons"));
                    if (!init) {
                        DEBUG_FATAL("Failed to find InitializeSingletons() in .so");
                    }

                    init(&EPOCH::Engine::Debugging::Debugger::GetInstance(), &EPOCH::Engine::Rendering::Renderer::GetInstance(),
                            &EPOCH::Engine::Core::Resources::ResourcesManager::GetInstance(),
                            &EPOCH::Engine::Rendering::CameraManager::GetInstance(),
                            &EPOCH::Engine::Time::TimeManager::GetInstance());

                    using StartFunc = void(*)();
                    StartFunc start = reinterpret_cast<StartFunc>(dlsym(handle, "EditorStart"));

                    if (!start) {
                        DEBUG_FATAL("Failed to find StartEditor() in .dll");
                        
                    }

                    start();

                #endif

                    DEBUG_LOG("Editor module loaded : " + modulePath);
            }

            void TickEditorModule(const std::string& modulePath){
                #if defined(_WIN32)
                    HMODULE hLib = GetModuleHandleA(modulePath.c_str());
                    if (!hLib) {
                        DEBUG_FATAL("Failed to load editor module's handle : " + modulePath);
                    }

                    using TickFunc = void(*)();
                    TickFunc start = reinterpret_cast<TickFunc>(
                        GetProcAddress(hLib, "EditorTick"));

                    if (!start) {
                        DEBUG_FATAL("Failed to find EditorTick() in .dll");
                    }

                    start();

                #else
                
                #endif
            }
            
            void CleanupEditorModule(const std::string& modulePath){
                #if defined(_WIN32)
                    HMODULE hLib = GetModuleHandleA(modulePath.c_str());
                    if (!hLib) {
                        DEBUG_FATAL("Failed to load editor module's handle : " + modulePath);
                    }

                    using TickFunc = void(*)();
                    TickFunc start = reinterpret_cast<TickFunc>(
                        GetProcAddress(hLib, "EditorCleanup"));

                    if (!start) {
                        DEBUG_FATAL("Failed to find EditorCleanup() in .dll");
                    }

                    start();

                #else
                
                #endif
            }

        private:

            ModuleLoader() = default;
            ~ModuleLoader() = default;
            ModuleLoader(const ModuleLoader&) = delete;
            ModuleLoader& operator=(const ModuleLoader&) = delete;
    };
}