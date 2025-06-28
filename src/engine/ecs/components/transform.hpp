#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/ecs/components/component.hpp"

namespace SHAME::Engine::ECS::Components
{
    class Transform : public Component{

        public:
        Transform(Objects::Actor *parent, uint32_t local_id, glm::vec3 position, glm::quat rotation, glm::vec3 scale);
        Transform(Objects::Actor *parent, uint32_t local_id);

        const glm::vec3& GetPosition() const { return position; };
        const glm::quat& GetRotation() const { return rotation; };
        const glm::vec3& GetScale() const { return scale; };

        void SetPosition(glm::vec3 position);
        void SetRotation(glm::quat rotation);
        void SetScale(glm::vec3 scale);

        void Translate(glm::vec3 deltaPosition);
        void Rotate(glm::quat angle);
        void Scale(glm::vec3 deltaScale);

        glm::vec3 Right() const { return right; };
        glm::vec3 Up() const { return up; };

        glm::mat4 GetMatrix();

        bool SetFromMatrix(const glm::mat4 &m);

        void Destroy() override{
            //TODO
        }

        private:


        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;

        glm::vec3 right = glm::vec3(1, 0, 0);
        glm::vec3 up = glm::vec3(0, 1, 0);

    };
}