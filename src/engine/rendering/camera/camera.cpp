#include "camera.hpp"

#include <iostream>

namespace SHAME::Engine::Rendering{
    
    Camera::Camera(int width, int height, glm::vec3 position, glm::vec3 rotation)
    {
        this->width = width;
        this->height = height;
        this->position = position;
        this->forward = rotation;
    }

    void Camera::UpdateSize(int new_width, int new_height)
    {
        this->width = new_width;
        this->height = new_height;
    }

    void Camera::UpdateMatrix(float nearPlane, float farPlane)
    {
        view = glm::mat4(1.0f);
        projection = glm::mat4(1.0f);

        view = glm::lookAt(position, position + forward, up);

        projection = glm::perspective(glm::radians(60.0f), float(width) / float(height), 0.1f, 1000.0f);
        cameraMatrix = projection * view;
    }

    glm::mat4 Camera::GetMatrix()
    {
        return cameraMatrix;
    }

    void Camera::Inputs(GLFWwindow* window, float speed)
    {
        // Movement input
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += speed * forward;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= glm::normalize(glm::cross(forward, up)) * speed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= speed * forward;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += glm::normalize(glm::cross(forward, up)) * speed;

        // Mouse input
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            if (firstClick)
            {
                originalmouseX = mouseX;
                originalmouseY = mouseY;
                firstClick = false;
            }

            double deltaX = mouseX - originalmouseX;
            double deltaY = originalmouseY - mouseY; // reversed Y

            originalmouseX = mouseX;
            originalmouseY = mouseY;

            yaw += deltaX * mouseSensitivity;
            pitch += deltaY * mouseSensitivity;

            pitch = glm::clamp(pitch, -89.0f, 89.0f);

            // Calculate new forward vector
            glm::vec3 direction;
            direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction.y = sin(glm::radians(pitch));
            direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            forward = glm::normalize(direction);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstClick = true;
        }
    }
}