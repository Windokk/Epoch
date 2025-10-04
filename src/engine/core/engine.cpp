#include "engine.hpp"

#include <iostream>
#include <string>
#include <thread>

#include "engine/core/resources/resources_manager.hpp"


using namespace std::chrono;

namespace Epoch::Engine::Core{
    
    using namespace Levels;
    using namespace Engine::Rendering;
    using namespace Physics;
    using namespace Filesystem;
    using namespace Audio;
    using namespace Input;
    using namespace Events;

    EngineInstance::EngineInstance(EngineCreationSettings settings)  
    {
        this->settings = settings;
        CreateWindow();
        FileManager::Init(settings.rootPath);
        PhysicsSystem::Init(settings.gravity);
        AudioManager::GetInstance().Init(100.0f);
        Renderer::GetInstance().Init(window);
        Resources::ResourcesManager::GetInstance().LoadResources(Filesystem::Path("project_resources"), Filesystem::Path("engine_resources"));
        Renderer::GetInstance().InitFramebuffers();
        GetInputManager().Init(window);
        EventDispatcher::GetInstance();

        float fixedDelta = 1.0f / 30.0f;

        Time::TimeManager::GetInstance().Init(fixedDelta);
    }

    void EngineInstance::CreateWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); 
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_SAMPLES, 4);
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
            DEBUG_FATAL("Failed to create GLFW window");
        }

        glfwSetWindowUserPointer(window, this);
        glfwMakeContextCurrent(window);
        glfwSetWindowSizeCallback(window, OnWindowResize);
        glfwSwapInterval(settings.vsync);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            DEBUG_FATAL("Failed to initialize GLAD");
        }

        
        const GLubyte* renderer = glGetString(GL_RENDERER);     // GPU
        const GLubyte* vendor   = glGetString(GL_VENDOR);       // GPU vendor
        const GLubyte* version  = glGetString(GL_VERSION);      // OpenGL version

        DEBUG_INFO("===== System infos =====");
        DEBUG_INFO(std::string("GPU Vendor : ") + reinterpret_cast<const char*>(vendor));
        DEBUG_INFO(std::string("GPU Renderer : ") + reinterpret_cast<const char*>(renderer));
        DEBUG_INFO(std::string("OpenGL Version : ") + reinterpret_cast<const char*>(version));

        // GLFW context version
        int major, minor, rev;
        glfwGetVersion(&major, &minor, &rev);
        DEBUG_INFO("GLFW Version : " + std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(rev));

        // Monitor and resolution
        int count = 0;
        GLFWmonitor** monitors = glfwGetMonitors(&count);
        DEBUG_INFO("Connected Monitors: " + std::to_string(count));

        for (int i = 0; i < count; ++i) {
            const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
            DEBUG_INFO("Monitor " + std::to_string(i) + ": "
                    + std::to_string(mode->width) + "x" + std::to_string(mode->height) + " @ " + std::to_string(mode->refreshRate) + "Hz");
        }
        // Window size
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        DEBUG_INFO("Window Size: " + std::to_string(width) + "x" + std::to_string(height));
        
    }

    void EngineInstance::DestroyWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void EngineInstance::Destroy()
    {
        GetInputManager().Shutdown();
        AudioManager::GetInstance().Shutdown();
        PhysicsSystem::Shutdown();
        LevelManager::GetInstance().UnloadAllLevels();
        Renderer::GetInstance().Shutdown();
        DestroyWindow();
    }

    bool EngineInstance::Run() {
        auto& time = Time::TimeManager::GetInstance();
        time.Tick();

        //PhysicsSystem::StepSimulation(time.GetFixedDeltaTime());

        glfwPollEvents();

        Renderer::GetInstance().Render();
        AudioManager::GetInstance().Tick();
        GetInputManager().Tick();

        LevelManager::GetInstance().Tick();

        glfwSwapBuffers(window);

        if(GetInputManager().WasKeyPressed(KEY_ESCAPE))
        {
            return false;
        }

        return true;
    }

    void EngineInstance::OnWindowResize(GLFWwindow *window, int width, int height)
    {
        Renderer::GetInstance().RescaleFramebuffers(width, height);
    }

}