#pragma once

#include <unordered_map>

#include <GLFW/glfw3.h>

#include "keys.hpp"

namespace SHAME::Engine::Input
{
    class InputManager{
        public:
            static InputManager& GetInstance() {
                static InputManager instance;
                return instance;
            }

            static void Init(GLFWwindow* window);
            static void Tick();
            static void Shutdown();
            static bool IsKeyDown(int key) {
                auto it = mCurrentKeyState.find(key);
                return it != mCurrentKeyState.end() && it->second;
            }

            static bool IsKeyUp(int key) {
                return !IsKeyDown(key);
            }

            static bool WasKeyPressed(int key) {
                return !mPreviousKeyState[key] && mCurrentKeyState[key];
            }

            static bool WasKeyReleased(int key) {
                return mPreviousKeyState[key] && !mCurrentKeyState[key];
            }

        private:
            InputManager() = default;
            ~InputManager() = default;
            InputManager(const InputManager&) = delete;
            InputManager& operator=(const InputManager&) = delete;

            static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
                if (key >= 0) {
                    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                        mCurrentKeyState[key] = true;
                    } else if (action == GLFW_RELEASE) {
                        mCurrentKeyState[key] = false;
                    }
                }
            }

            static std::unordered_map<int, bool> mCurrentKeyState;
            static std::unordered_map<int, bool> mPreviousKeyState;
    };
}
