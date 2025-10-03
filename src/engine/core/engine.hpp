#pragma once

#include <stdexcept>

#include "engine/filesystem/filesystem.hpp"
#include "engine/levels/level_manager.hpp"
#include "engine/ecs/objects/actors/actor.hpp"
#include "engine/inputs/input_manager.hpp"
#include "engine/time/time_manager.hpp"

namespace Epoch::Engine::Core {
    
    struct EngineCreationSettings{
        //WINDOW
        int windowWidth = 800;
        int windowHeight = 800;
        bool fullscreen = false;
        bool vsync = true;
        int targetFPS = 60;

        //FILESYSTEM
        std::string rootPath = "";

        //PHYSICS
        glm::vec3 gravity = glm::vec3(0, -9.81f, 0);
    };

    class EngineInstance{
        public:
        EngineInstance(EngineCreationSettings settings = {});
        bool shouldEnd() { return glfwWindowShouldClose(window); };
        void Destroy();
        bool Run();

        GLFWwindow* GetGLFWWindow() { return window; }

        static void OnWindowResize(GLFWwindow *window, int width, int height);

        EngineCreationSettings settings;

    private:
        void CreateWindow();
        void DestroyWindow();

        GLFWwindow* window;

    };
}