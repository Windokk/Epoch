#include "input_manager.hpp"


namespace EPOCH::Engine::Input{

    InputManager gSharedInputManager;

    void InputManager::Init(GLFWwindow* window)
    {

        win = window;

        glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int sc, int action, int mods) {
            GetInputManager().KeyCallback(key, action);
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int mods) {
            GetInputManager().MouseCallback(button, action);
        });
    }

    void InputManager::Tick()
    {
        mPreviousKeyState = mCurrentKeyState;
        mPreviousMouseState = mCurrentMouseState;
    }

    void InputManager::Shutdown()
    {
        mCurrentKeyState.clear();
        mPreviousKeyState.clear();
        mCurrentMouseState.clear();
        mPreviousMouseState.clear();
    }

    void InputManager::KeyCallback(int key, int action)
    {
        if (key >= 0) {
            if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                mCurrentKeyState[key] = true;
            } else if (action == GLFW_RELEASE) {
                mCurrentKeyState[key] = false;
            }
        }
    }

    void InputManager::MouseCallback(int button, int action)
    {
        if (button >= 0) {
            if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                mCurrentMouseState[button] = true;
            } else if (action == GLFW_RELEASE) {
                mCurrentMouseState[button] = false;
            }
        }
    }

    bool InputManager::IsKeyDown(int key) {
        auto it = mCurrentKeyState.find(key);
        return it != mCurrentKeyState.end() && it->second;
    }

    bool InputManager::IsKeyUp(int key) {
        return !IsKeyDown(key);
    }

    bool InputManager::WasKeyPressed(int key) {
        return !mPreviousKeyState[key] && mCurrentKeyState[key];
    }

    bool InputManager::WasKeyReleased(int key) {
        return mPreviousKeyState[key] && !mCurrentKeyState[key];
    }

    bool InputManager::IsMouseDown(int button)
    {
        auto it = mCurrentMouseState.find(button);
        return it != mCurrentMouseState.end() && it->second;
    }

    bool InputManager::IsMouseUp(int button)
    {
        return !IsMouseDown(button);
    }

    bool InputManager::WasMousePressed(int button)
    {
        return !mPreviousMouseState[button] && mCurrentMouseState[button];
    }

    bool InputManager::WasMouseReleased(int button)
    {
        return mPreviousMouseState[button] && !mCurrentMouseState[button];
    }

    void InputManager::SetCursorVisibility(bool visible)
    {
        glfwSetInputMode(win, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
    }

    void InputManager::GetCursorPos(double *x, double *y)
    {
        glfwGetCursorPos(win, x, y);
    }

    void InputManager::SetCursorPos(double x, double y)
    {
        glfwSetCursorPos(win, x, y);
    }
};