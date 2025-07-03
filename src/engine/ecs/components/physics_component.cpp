#include "physics_component.hpp"

#include "engine/ecs/objects/actors/actor.hpp"

#include <iostream>

namespace SHAME::Engine::ECS::Components{
    PhysicsComponent::PhysicsComponent(Objects::Actor *parent, uint32_t local_id) : Component(parent, local_id)
    {
        
    }

    void PhysicsComponent::CreateBody(Physics::PhysicsShape shape, glm::vec3 scale, EMotionType motionType)
    {
        JPH::ShapeRefC shapeRef;

        this->scale = scale;
        this->type = shape;

        switch (shape) {
            case Physics::PhysicsShape::SPHERE: {
                JPH::SphereShapeSettings sphereSettings(scale.x == 0 ? (scale.y == 0 ? scale.z : scale.y) : scale.x);
                auto shapeResult = sphereSettings.Create();
                if (shapeResult.HasError()) {
                    std::cerr << "Failed to create sphere shape: " << shapeResult.GetError() << "\n";
                    return;
                }

                shapeRef = shapeResult.Get();
                debugShape = new Rendering::DebugSphere((scale.x == 0 ? (scale.y == 0 ? scale.z : scale.y) : scale.x), COL_RGBA(0, 1, 1, 1));
                break;
            }
            case Physics::PhysicsShape::BOX:{
                JPH::BoxShapeSettings boxSettings(JPH::Vec3(scale.x, scale.y, scale.z));
                auto shapeResult = boxSettings.Create();
                if (shapeResult.HasError()) { 
                    std::cerr << "Failed to create cube shape: " << shapeResult.GetError() << "\n";
                    return;
                }

                shapeRef = shapeResult.Get();
                debugShape = new Rendering::DebugBox(glm::vec3(scale.x, scale.y, scale.z), COL_RGBA(0, 1, 1, 1));
                break;
            }
            case Physics::PhysicsShape::CAPSULE: {
                JPH::CapsuleShapeSettings capsuleSettings(scale.y, scale.x == 0 ? scale.z : scale.x);
                auto shapeResult = capsuleSettings.Create();
                if (shapeResult.HasError()) { 
                    std::cerr << "Failed to create capsule shape: " << shapeResult.GetError() << "\n";
                    return;
                }

                shapeRef = shapeResult.Get();
                debugShape = new Rendering::DebugCapsule(scale.y, scale.x == 0 ? scale.z : scale.x, COL_RGBA(0, 1, 1, 1));
                break;
            }
            case Physics::PhysicsShape::CYLINDER:{
                JPH::CylinderShapeSettings cylinderSettings(scale.y, scale.x == 0 ? scale.z : scale.x);
                auto shapeResult = cylinderSettings.Create();
                if (shapeResult.HasError()) { 
                    std::cerr << "Failed to create cylinder shape: " << shapeResult.GetError() << "\n";
                    return;
                }

                shapeRef = shapeResult.Get();
                debugShape = new Rendering::DebugCylinder(scale.y, scale.x == 0 ? scale.z : scale.x, COL_RGBA(0, 1, 1, 1));
                break;
            }

            default:
                std::cerr << "Unsupported shape type!\n";
                return;
        }

        glm::vec3 pos = parent->GetComponent<Transform>().GetPosition();
        glm::quat rot = parent->GetComponent<Transform>().GetRotation();
        
        // Body settings
        JPH::BodyCreationSettings settings(
            shapeRef,
            JPH::RVec3(pos.x, pos.y, pos.z),        // position
            JPH::Quat(rot.x, rot.y, rot.z, rot.w),                 // rotation
            motionType,              // motion type
            1
        );

        mBodyID = Physics::PhysicsSystem::CreateBody(settings);
    }

    void PhysicsComponent::Update(){

        JPH::RVec3 pos = Physics::PhysicsSystem::m_physicsSystem.GetBodyInterface().GetCenterOfMassPosition(mBodyID);
        JPH::Quat rot = Physics::PhysicsSystem::m_physicsSystem.GetBodyInterface().GetRotation(mBodyID);

        this->parent->GetComponent<Transform>().SetPosition(glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ()));
        this->parent->GetComponent<Transform>().SetRotation(glm::vec3(rot.GetEulerAngles().GetX(), rot.GetEulerAngles().GetY(), rot.GetEulerAngles().GetZ()));
    }
}