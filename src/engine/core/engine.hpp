#pragma once

#include <stdexcept>

#include <chrono>

#include <glad/glad.h>

#include "engine/levels/level_manager.hpp"
#include "engine/ecs/objects/actors/actor.hpp"

namespace SHAME::Engine::Core {
    class EngineInstance{
        public:
        EngineInstance();
        bool shouldEnd() { return glfwWindowShouldClose(window); };
        void Destroy();
        void Run();

        GLFWwindow* GetGLFWWindow() { return window; }

        static void OnWindowResize(GLFWwindow *window, int width, int height);

    private:
        void CreateWindow();
        void DestroyWindow();

        GLFWwindow* window;

        std::chrono::_V2::system_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
    };
}