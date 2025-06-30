#include "engine.hpp"

#include <iostream>
#include <string>

#include "engine/filesystem/filesystem.hpp"

using namespace std::chrono;

namespace SHAME::Engine::Core{
    
    using namespace Levels;
    using namespace Engine::Rendering;
    using namespace Physics;
    using namespace Filesystem;
    using namespace Audio;

    EngineInstance::EngineInstance()  
    {
        FileManager::InitializeSession();

        CreateWindow();
        Renderer::Init(window);
        PhysicsSystem::Init();
        AudioManager::Init(100.0f);
    }

    void EngineInstance::CreateWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); 
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(800, 800, "SHA.me", NULL, NULL);
        if (window == NULL)
        {
            glfwTerminate();
            throw std::runtime_error("[ERROR] [ENGINE/CORE] : Failed to create GLFW window");
        }

        glfwSetWindowUserPointer(window, this);
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, OnWindowResize);
        glfwSwapInterval(1);
        

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw std::runtime_error("[ERROR] [ENGINE/CORE] : Failed to initialize GLAD");
        }
    }

    void EngineInstance::DestroyWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void EngineInstance::Destroy()
    {
        AudioManager::Shutdown();
        PhysicsSystem::Shutdown();
        LevelManager::UnloadAllLevels();
        Renderer::Shutdown();
        DestroyWindow();
    }

    void EngineInstance::Run()
    {
        auto currentTime = std::chrono::high_resolution_clock::now();

        // Use floating-point duration in milliseconds
        std::chrono::duration<float, std::milli> duration = currentTime - lastTime;
        float ms = duration.count();
        /*
        if (ms > 0.0f)
            std::cout << (1000.0f / ms) << " FPS" << std::endl;
        else
            std::cout << "Very fast frame (<1ms), can't compute FPS accurately" << std::endl;*/

        Renderer::Render();
        PhysicsSystem::StepSimulation(1.0f / 60.0f);
        AudioManager::Tick();

        lastTime = currentTime;
    }

    void EngineInstance::OnWindowResize(GLFWwindow *window, int width, int height)
    {
        
    }

}