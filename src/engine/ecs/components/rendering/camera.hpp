#pragma once

#include "engine/rendering/utils.hpp"

#include "engine/ecs/components/core/component.hpp"

namespace EPOCH::Engine::ECS::Components {
    
    class Camera : public Component
    {
        public :
            
            Camera(Objects::Actor *parent, uint32_t local_id);

            void Init(int width, int height, float near, float far);

            void UpdateSize(int new_width, int new_height);

            void UpdateMatrix();

            glm::mat4 GetMatrix()
            {
                return cameraMatrix;
            }

            glm::mat4 GetView()
            {
                return view;
            }

            glm::mat4 GetProjection()
            {
                return projection;
            }

            glm::vec2 GetSize()
            {
                return glm::vec2(width, height);
            }

            

        private:

            // Matrices
            glm::mat4 view;
            glm::mat4 projection;
            glm::mat4 cameraMatrix = glm::mat4(1.0f);

            // Mouse inputs
            float mouseSensitivity = 0.1f;
            bool firstClick = true;
            double originalmouseX = 0.0, originalmouseY = 0.0;
            double lockedMouseY = 0.0;
            double lockedMouseX = 0.0;

            // Store the width and height of the window
            int width = 0;
            int height = 0;

            float farPlane = 0;
            float nearPlane = 0;

            bool canInteract = true;

            //void Inputs(GLFWwindow* window, float speed = 0.6f);
    };
}