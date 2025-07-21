#pragma once

#include <unordered_map>
#include <iostream>

#include <GLFW/glfw3.h>

#include "keys.hpp"
#include "engine/debugging/debugger.hpp"

namespace SHAME::Engine::Input
{
    class InputManager{
        public:

            void Init(GLFWwindow* window);
            void Tick();
            void Shutdown();

            bool IsKeyDown(int key);
            bool IsKeyUp(int key);
            bool WasKeyPressed(int key);
            bool WasKeyReleased(int key);

            bool IsMouseDown(int button);
            bool IsMouseUp(int button);
            bool WasMousePressed(int button);
            bool WasMouseReleased(int button);

            void SetCursorVisibility(bool visible);
            void GetCursorPos(double* x, double* y);
            void SetCursorPos(double x, double y);
            

        private:

            void KeyCallback(int key,  int action);
            void MouseCallback(int button, int action);

            std::unordered_map<int, bool> mCurrentKeyState;
            std::unordered_map<int, bool> mPreviousKeyState;

            std::unordered_map<int, bool> mCurrentMouseState;
            std::unordered_map<int, bool> mPreviousMouseState;
            
            GLFWwindow* win = nullptr;
    };
    
    extern InputManager gSharedInputManager;

    #if defined(BUILD_ENGINE)

        // Used by the EXE/engine
        inline InputManager& GetInputManager() {
            return gSharedInputManager;
        }

    #else
        // Used by the DLL
        inline InputManager* gSharedInputManagerPtr = nullptr;

        inline void SetInputManager(InputManager* ptr) {
            gSharedInputManagerPtr = ptr;
        }

        inline InputManager& GetInputManager() {
            if (!gSharedInputManagerPtr)
                DEBUG_FATAL("InputManager pointer not initialized!");
            return *gSharedInputManagerPtr;
        }

    #endif
}
