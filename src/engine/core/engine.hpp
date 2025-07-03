#pragma once

#include <stdexcept>
#include <chrono>

#include "engine/filesystem/filesystem.hpp"
#include "engine/levels/level_manager.hpp"
#include "engine/ecs/objects/actors/actor.hpp"
#include "engine/input/input_manager.hpp"

namespace SHAME::Engine::Core {
    
    struct EngineCreationSettings{
        int windowWidth = 800;
        int windowHeight = 800;
        bool fullscreen = false;
        std::string rootPath = "";
    };

    class EngineInstance{
        public:
        EngineInstance(EngineCreationSettings settings = {});
        bool shouldEnd() { return glfwWindowShouldClose(window); };
        void Destroy();
        void Run();

        GLFWwindow* GetGLFWWindow() { return window; }

        static void OnWindowResize(GLFWwindow *window, int width, int height);

        EngineCreationSettings settings;

    private:
        void CreateWindow();
        void DestroyWindow();

        GLFWwindow* window;

        std::chrono::_V2::system_clock::time_point lastTime = std::chrono::high_resolution_clock::now();

    };
}