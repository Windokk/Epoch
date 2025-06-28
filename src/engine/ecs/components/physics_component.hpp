#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/Body.h>

#include "engine/physics/physics_system.hpp"

#include "engine/ecs/components/component.hpp"

#include "engine/rendering/utils.hpp"

#include "engine/rendering/debug/debug.hpp"

namespace SHAME::Engine::ECS::Components
{

    class PhysicsComponent : public Component {
        private:
        JPH::BodyID mBodyID = JPH::BodyID();  // default invalid ID
        Physics::PhysicsShape type;
        glm::vec3 scale;
        Rendering::DebugShape* debugShape = nullptr;

        public:
            PhysicsComponent(Objects::Actor *parent, uint32_t local_id);

            void CreateBody(Physics::PhysicsShape shape, glm::vec3 scale, EMotionType motionType);

            void Update();

            void RemoveBody() {
                if (!mBodyID.IsInvalid()) {
                    Physics::PhysicsSystem::RemoveBody(mBodyID);
                    mBodyID = JPH::BodyID();
                }
            }
            
            Physics::PhysicsShape GetType() { return type; }
            Rendering::DebugShape* GetDebugShape()  { return debugShape; }
            JPH::BodyID GetBodyID() const { return mBodyID; }
    };
}