#include "camera.hpp"

#include <iostream>

#include "engine/ecs/objects/actors/actor.hpp"

namespace EPOCH::Engine::ECS::Components {
    
    Camera::Camera(Objects::Actor *parent, uint32_t local_id) : Component(parent, local_id)
    {

    }

    void Camera::Init(int width, int height, float near, float far)
    {
        this->width = width;
        this->height = height;
        this->nearPlane = near;
        this->farPlane = far;
    }

    void Camera::UpdateSize(int new_width, int new_height)
    {
        if(!activated)
            return;

        this->width = new_width;
        this->height = new_height;
    }

    void Camera::UpdateMatrix()
    {
        if(!activated)
            return;
            
        view = glm::mat4(1.0f);
        projection = glm::mat4(1.0f);

        std::shared_ptr<Transform> tr = parent->transform;

        view = glm::lookAt(tr->GetPosition(), tr->GetPosition() + tr->GetForward(), tr->GetUp());

        projection = glm::perspective(glm::radians(60.0f), float(width) / float(height), nearPlane, farPlane);
        cameraMatrix = projection * view;
    }

    /*void Camera::Inputs(GLFWwindow* window, float speed)
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

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            if (firstClick)
            {
                lockedMouseX = mouseX;
                lockedMouseY = mouseY;
                firstClick = false;
                return; // Skip delta on first click
            }

            double deltaX = mouseX - lockedMouseX;
            double deltaY = lockedMouseY - mouseY; // reversed Y

            rotation.y += deltaX * mouseSensitivity;
            rotation.x += deltaY * mouseSensitivity;
            rotation.x = glm::clamp(rotation.y, -89.0f, 89.0f);

            glm::vec3 direction;
            direction.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
            direction.y = sin(glm::radians(rotation.x));
            direction.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
            forward = glm::normalize(direction);

            // Reset cursor back to locked position every frame
            glfwSetCursorPos(window, lockedMouseX, lockedMouseY);
        }
        else
        {
            firstClick = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }*/
}