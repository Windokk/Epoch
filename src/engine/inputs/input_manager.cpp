#include "input_manager.hpp"


namespace SHAME::Engine::Input{

    std::unordered_map<int, bool> InputManager::mCurrentKeyState;
    std::unordered_map<int, bool> InputManager::mPreviousKeyState;

    void InputManager::Init(GLFWwindow* window)
    {
        GetInstance();
        glfwSetKeyCallback(window, KeyCallback);
    }

    void InputManager::Tick()
    {
        mPreviousKeyState = mCurrentKeyState;
    }

    void InputManager::Shutdown()
    {

    }

};