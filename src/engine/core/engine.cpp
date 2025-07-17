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
    using namespace Events;

    EngineInstance::EngineInstance(EngineCreationSettings settings)  
    {
        this->settings = settings;
        CreateWindow();
        FileManager::Init(settings.rootPath);
        Renderer::Init(window);
        PhysicsSystem::Init(settings.gravity);
        AudioManager::Init(100.0f);
        InputManager::Init(window);
        EventDispatcher::GetInstance();

        float fixedDelta = 1.0f / 30.0f;

        Time::TimeManager::GetInstance().Init(fixedDelta);
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
        glfwSwapInterval(settings.vsync);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw std::runtime_error("[ERROR] [ENGINE/CORE] : Failed to initialize GLAD");
        }

        
        const GLubyte* renderer = glGetString(GL_RENDERER);     // GPU
        const GLubyte* vendor   = glGetString(GL_VENDOR);       // GPU vendor
        const GLubyte* version  = glGetString(GL_VERSION);      // OpenGL version

        std::cout << "[INFO] [ENGINE/CORE/SYSTEM_INFOS] ===== System infos =====\n";
        std::cout << "[INFO] [ENGINE/CORE/SYSTEM_INFOS] GPU Vendor: " << vendor << "\n";
        std::cout << "[INFO] [ENGINE/CORE/SYSTEM_INFOS] GPU Renderer: " << renderer << "\n";
        std::cout << "[INFO] [ENGINE/CORE/SYSTEM_INFOS] OpenGL Version: " << version << "\n";

        // GLFW context version (you asked for)
        int major, minor, rev;
        glfwGetVersion(&major, &minor, &rev);
        std::cout << "[INFO] [ENGINE/CORE/SYSTEM_INFOS] GLFW Version: " << major << "." << minor << "." << rev << "\n";

        // Monitor and resolution
        int count;
        GLFWmonitor** monitors = glfwGetMonitors(&count);
        std::cout << "[INFO] [ENGINE/CORE/SYSTEM_INFOS] Connected Monitors: " << count << "\n";

        for (int i = 0; i < count; ++i) {
            const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
            std::cout << "[INFO] [ENGINE/CORE/SYSTEM_INFOS] Monitor " << i << ": "
                    << mode->width << "x" << mode->height << " @ " << mode->refreshRate << "Hz\n";
        }
        // Window size
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        std::cout << "[INFO] [ENGINE/CORE/SYSTEM_INFOS] Window Size: " << width << "x" << height << "\n";
        
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

    void EngineInstance::Run() {
        auto& time = Time::TimeManager::GetInstance();
        time.Tick();

        PhysicsSystem::StepSimulation(time.GetFixedDeltaTime());

        Renderer::Render();
        AudioManager::Tick();
        InputManager::Tick();
        LevelManager::Tick();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void EngineInstance::OnWindowResize(GLFWwindow *window, int width, int height)
    {
    }

}