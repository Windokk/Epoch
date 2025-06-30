#include "physics_system.hpp"

#include "engine/levels/level_manager.hpp"

namespace SHAME::Engine::Physics {

    // Constants
    static constexpr uint cMaxBodies = 1024;
    static constexpr uint cNumBodyMutexes = 0;
    static constexpr uint cMaxBodyPairs = 1024;
    static constexpr uint cMaxContactConstraints = 1024;
    static constexpr uint cTempAllocatorSize = 10 * 1024 * 1024;

    JPH::PhysicsSystem PhysicsSystem::m_physicsSystem;

    JPH::TempAllocatorImpl* PhysicsSystem::m_tempAllocator = nullptr;
    JPH::JobSystem* PhysicsSystem::m_jobSystem = nullptr;

    ObjectLayerPairFilterImpl PhysicsSystem::m_objectLayerFilter;
    ObjectVsBroadPhaseLayerFilterImpl PhysicsSystem::m_objectVsBroadphase;
    BPLayerInterfaceImpl PhysicsSystem::m_broadPhaseLayer;

    MyContactListener PhysicsSystem::m_contactListener;
    MyBodyActivationListener PhysicsSystem::m_activationListener;

    void PhysicsSystem::Init()
    {
        JPH::RegisterDefaultAllocator();

        JPH::Trace = &TraceImpl;

        JPH_IF_ENABLE_ASSERTS(
            JPH::AssertFailed = [](const char* inExpr, const char* inMsg, const char* inFile, uint inLine)
            {
                std::cerr << inFile << ":" << inLine << ": (" << inExpr << ") " << (inMsg ? inMsg : "") << std::endl;
                return true;
            };
        )

        // Required factory setup
        JPH::Factory::sInstance = new JPH::Factory();
        JPH::RegisterTypes();

        // Allocator & job system
        m_tempAllocator = new JPH::TempAllocatorImpl(cTempAllocatorSize);
        m_jobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

        // Initialize physics system
        m_physicsSystem.Init(
            cMaxBodies,
            cNumBodyMutexes,
            cMaxBodyPairs,
            cMaxContactConstraints,
            m_broadPhaseLayer,
            m_objectVsBroadphase,
            m_objectLayerFilter
        );

        m_physicsSystem.SetBodyActivationListener(&m_activationListener);
        m_physicsSystem.SetContactListener(&m_contactListener);
    }

    void PhysicsSystem::Shutdown()
    {
        for (auto& physicsBody : Levels::LevelManager::GetLevelAt(0)->physicsBodies){
            physicsBody->RemoveBody();
        }

        delete m_jobSystem;
        m_jobSystem = nullptr;

        delete m_tempAllocator;
        m_tempAllocator = nullptr;

        JPH::UnregisterTypes();
        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
    }

    void PhysicsSystem::StepSimulation(float deltaTime)
    {
        m_physicsSystem.Update(deltaTime, 1, m_tempAllocator, m_jobSystem);
        
        for (auto& physicsBody : Levels::LevelManager::GetLevelAt(0)->physicsBodies){
            physicsBody->Update();
        }
    }

    JPH::BodyID PhysicsSystem::CreateBody(const JPH::BodyCreationSettings &settings, JPH::EActivation activation)
    {
        JPH::BodyInterface& bodyInterface = m_physicsSystem.GetBodyInterface();

        // Create body
        JPH::Body* body = bodyInterface.CreateBody(settings);
        if (body == nullptr)
        {
            std::cerr << "[ERROR] [ENGINE/PHYSICS] : Failed to create body!" << std::endl;
            return JPH::BodyID();
        }

        // Add to worlds
        bodyInterface.AddBody(body->GetID(), activation);
        return body->GetID();
    }

    void PhysicsSystem::RemoveBody(JPH::BodyID id)
    {
        JPH::BodyInterface& bodyInterface = m_physicsSystem.GetBodyInterface();
        bodyInterface.RemoveBody(id);
        bodyInterface.DestroyBody(id);
    }
}