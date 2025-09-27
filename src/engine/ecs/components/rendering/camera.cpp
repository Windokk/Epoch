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

        projection = glm::perspective(glm::radians(fov), float(width) / float(height), nearPlane, farPlane);
        cameraMatrix = projection * view;
    }
}