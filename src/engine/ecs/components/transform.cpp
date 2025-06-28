#include "transform.hpp"

#include <glm/gtx/matrix_decompose.hpp>

#include "engine/rendering/renderer/renderer.hpp"
#include "engine/ecs/objects/actors/actor.hpp"
#include "engine/ecs/components/light_component.hpp"

namespace SHAME::Engine::ECS::Components{
    
    Transform::Transform(Objects::Actor *parent, uint32_t local_id, glm::vec3 position, glm::quat rotation, glm::vec3 scale) : Component(parent, local_id)
    {
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;
    }

    Transform::Transform(Objects::Actor *parent, uint32_t local_id) : Component(parent, local_id)
    {
        this->position = glm::vec3(0,0,0);
        this->rotation = glm::quat(glm::vec3(0,0,0));
        this->scale = glm::vec3(1,1,1);
    }

    void Transform::SetPosition(glm::vec3 position)
    {
		glm::vec3 oldpos = this->position;
        this->position = position;

		if(parent->HasComponent<Light>()){
			for(auto& light : parent->GetComponents<Light>()){
				light->SetPosition(glm::vec3(this->position.x, this->position.y, this->position.z));
			}
		}

		if(oldpos != position){
			Rendering::Renderer::ReorderDrawList();
		}
    }

    void Transform::SetRotation(glm::quat rotation)
    {
        this->rotation = rotation;

		
		if(parent->HasComponent<Light>()){
			for(auto& light : parent->GetComponents<Light>()){
				light->SetDirection(glm::eulerAngles(this->rotation));
			}
		}
    }

    void Transform::SetScale(glm::vec3 scale)
    {
        this->scale = scale;
    }

    void Transform::Translate(glm::vec3 deltaPosition)
    {
        this->position += deltaPosition;

		if(parent->HasComponent<Light>()){
			for(auto& light : parent->GetComponents<Light>()){
				light->SetPosition(glm::vec3(this->position.x, this->position.y, this->position.z));
			}
		}
		
		if(deltaPosition.z != 0){
			Rendering::Renderer::ReorderDrawList();
		}
    }

    void Transform::Rotate(glm::quat angle)
    {
        this->rotation = angle * this->rotation;

		
		if(parent->HasComponent<Light>()){
			for(auto& light : parent->GetComponents<Light>()){
				light->SetDirection(glm::eulerAngles(this->rotation));
			}
		}
    }

    void Transform::Scale(glm::vec3 deltaScale)
    {
        this->scale += deltaScale;
    }

    glm::mat4 Transform::GetMatrix()
    {
        return glm::translate(glm::mat4(1.0f), position)
            * glm::toMat4(rotation)
            * glm::scale(glm::mat4(1.0f), scale);
    }

    bool Transform::SetFromMatrix(const glm::mat4& m)
    {
        // From glm::decompose in matrix_decompose.inl
		using T = float;

		glm::mat4 localMatrix(m);

		// Normalize the matrix.
		if (glm::epsilonEqual(localMatrix[3][3], static_cast<float>(0), glm::epsilon<T>()))
			return false;

		// First, isolate perspective.  This is the messiest.
		if (
			glm::epsilonNotEqual(localMatrix[0][3], static_cast<T>(0), glm::epsilon<T>()) ||
			glm::epsilonNotEqual(localMatrix[1][3], static_cast<T>(0), glm::epsilon<T>()) ||
			glm::epsilonNotEqual(localMatrix[2][3], static_cast<T>(0), glm::epsilon<T>()))
		{
			// Clear the perspective partition
			localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = static_cast<T>(0);
			localMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).

		if(glm::vec3(localMatrix[3]) != this->position){
			position = glm::vec3(localMatrix[3]);

			if(parent->HasComponent<Light>()){
				for(auto& light : parent->GetComponents<Light>()){
					light->SetPosition(glm::vec3(this->position.x, this->position.y, this->position.z));
				}
			}
		}
		

		localMatrix[3] = glm::vec4(0, 0, 0, localMatrix[3].w);

		glm::vec3 Row[3], Pdum3;

		// Now get scale and shear.
		for (glm::length_t i = 0; i < 3; ++i)
			for (glm::length_t j = 0; j < 3; ++j)
				Row[i][j] = localMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);
		Row[0] = glm::detail::scale(Row[0], static_cast<T>(1));
		scale.y = length(Row[1]);
		Row[1] = glm::detail::scale(Row[1], static_cast<T>(1));
		scale.z = length(Row[2]);
		Row[2] = glm::detail::scale(Row[2], static_cast<T>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

        glm::vec3 localRot;

		localRot.y = asin(-Row[0][2]);
		if (cos(localRot.y) != 0) {
			localRot.x = atan2(Row[1][2], Row[2][2]);
			localRot.z = atan2(Row[0][1], Row[0][0]);
		}
		else {
			localRot.x = atan2(-Row[2][0], Row[1][1]);
			localRot.z = 0;
		}

		if(glm::quat(localRot) != this->rotation){
			rotation = glm::quat(localRot);

			if(parent->HasComponent<Light>()){
				for(auto& light : parent->GetComponents<Light>()){
					light->SetDirection(glm::eulerAngles(this->rotation));
				}
			}
		}
        

		return true;
    }
}
