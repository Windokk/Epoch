#pragma once

#include "engine/rendering/utils.hpp"

#include "engine/rendering/shader/shader.hpp"

namespace SHAME::Engine::Rendering {
    class Camera
    {
        
        glm::vec3 forward;
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 view;
        glm::mat4 projection;

        float yaw = -90.0f;
        float pitch = 0.0f;
        float mouseSensitivity = 0.1f;
        
        glm::mat4 cameraMatrix = glm::mat4(1.0f);
        
        bool firstClick = true;
        double originalmouseX = 0.0, originalmouseY = 0.0;
        // Stores the width and height of the window
        int width;
        int height;

        public :

        bool canInteract = true;
        glm::vec3 position;
        float zoom = 10.0f;

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

        Camera(int width, int height, glm::vec3 position, glm::vec3 rotation);

        void UpdateSize(int new_width, int new_height);

        void UpdateMatrix(float nearPlane, float farPlane);

        glm::mat4 GetMatrix();

        void Inputs(GLFWwindow* window, float speed = 0.6f);
    };
}