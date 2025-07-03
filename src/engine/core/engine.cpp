#include "engine.hpp"

#include <iostream>
#include <string>


using namespace std::chrono;

namespace SHAME::Engine::Core{
    
    using namespace Levels;
    using namespace Engine::Rendering;
    using namespace Physics;
    using namespace Filesystem;
    using namespace Audio;
    using namespace Input;

    EngineInstance::EngineInstance(EngineCreationSettings settings)  
    {
        this->settings = settings;
        CreateWindow();
        FileManager::Init(settings.rootPath);
        Renderer::Init(window);
        PhysicsSystem::Init();
        AudioManager::Init(100.0f);
        InputManager::Init(window);
    }

    void EngineInstance::CreateWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); 
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        if(settings.fullscreen){
            GLFWmonitor* primary = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(primary);
            window = glfwCreateWindow(mode->width, mode->height, "SHA.me", primary, nullptr);
        }
        else{
            window = glfwCreateWindow(settings.windowWidth, settings.windowHeight, "SHA.me", NULL, NULL);
        }
        
        if (window == NULL)
        {
            glfwTerminate();
            throw std::runtime_error("[ERROR] [ENGINE/CORE] : Failed to create GLFW window");
        }

        glfwSetWindowUserPointer(window, this);
        glfwMakeContextCurrent(window);
        glfwSetWindowSizeCallback(window, OnWindowResize);
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
        InputManager::Shutdown();
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
        //PhysicsSystem::StepSimulation(1.0f / 60.0f);
        AudioManager::Tick();
        InputManager::Tick();

        lastTime = currentTime;
    }

    void EngineInstance::OnWindowResize(GLFWwindow *window, int width, int height)
    {
    }

}