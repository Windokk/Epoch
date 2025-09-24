#pragma once

#include "engine/physics/physics_system.hpp"

#include "engine/ecs/objectID.hpp"

#include <unordered_map>
#include <functional>
#include <typeindex>
#include <vector>

namespace EPOCH::Engine::ECS::Components{
    class PhysicsBody;
}

namespace EPOCH::Engine::Events {

    struct Event {
        virtual ~Event() = default;
        ECS::ObjectID sourceObjectID;
        explicit Event(ECS::ObjectID source) : sourceObjectID(source) {}
    };

    // Common Event Definitions
    struct KeyPressedEvent : public Event {
        int keyCode;
        bool repeated;
        KeyPressedEvent(const int key, const bool rep, ECS::ObjectID source) : keyCode(key), repeated(rep), Event(source) {}
    };

    struct ContactAddedEvent : public Event {
        const ECS::Components::PhysicsBody& otherBody;
        const ContactManifold &contactManifold;
        ContactSettings &contactSettings;
        ContactAddedEvent(const ECS::Components::PhysicsBody& b2, const ContactManifold &manifold, ContactSettings &settings, ECS::ObjectID source)
             : otherBody(b2), contactManifold(manifold), contactSettings(settings), Event(source) {}
    };

    struct ContactPersistedEvent : public Event {
        const ECS::Components::PhysicsBody& otherBody;
        const ContactManifold &contactManifold;
        ContactSettings &contactSettings;
        ContactPersistedEvent(const ECS::Components::PhysicsBody& b2, const ContactManifold &manifold, ContactSettings &settings, ECS::ObjectID source)
             : otherBody(b2), contactManifold(manifold), contactSettings(settings), Event(source) {}
    };

    struct ContactRemovedEvent : public Event {
        const ECS::Components::PhysicsBody& otherBody;
        ContactRemovedEvent(
        const ECS::Components::PhysicsBody& b2, ECS::ObjectID source) : otherBody(b2), Event(source) {}
    };

    // EventDispatcher
    class EventDispatcher {
        public:
            static EventDispatcher& GetInstance() {
                static EventDispatcher instance;
                return instance;
            }

            using ComponentID = uint32_t;

            // Subscriptions
            template<typename T>
            static void subscribeGlobal(std::function<void(const T&)> callback) {
                globalSubscribers[typeid(T)].emplace_back([cb = std::move(callback)](const Event& e) {
                    cb(static_cast<const T&>(e));
                });
            }

            template<typename T>
            static void subscribeToLevel(int levelID, std::function<void(const T&)> callback) {
                levelSubscribers[levelID][typeid(T)].emplace_back([cb = std::move(callback)](const Event& e) {
                    cb(static_cast<const T&>(e));
                });
            }

            template<typename T>
            static void subscribeToActor(ECS::ObjectID actorID, std::function<void(const T&)> callback) {
                actorSubscribers[actorID][typeid(T)].emplace_back([cb = std::move(callback)](const Event& e) {
                    cb(static_cast<const T&>(e));
                });
            }

            template<typename T>
            void subscribeToComponent(ComponentID componentID, std::function<void(const T&)> callback) {
                componentSubscribers[componentID][typeid(T)].emplace_back(
                    [cb = std::move(callback)](const Event& e) {
                        cb(static_cast<const T&>(e));
                    }
                );
            }

            // Emission
            
            template<typename T>
            static void emitGlobal(const T& event) {
                dispatchTo(globalSubscribers, event);
            }

            template<typename T>
            static void emitToLevel(int levelID, const T& event) {
                auto it = levelSubscribers.find(levelID);
                if (it != levelSubscribers.end()) {
                    dispatchTo(it->second, event);
                }
            }

            template<typename T>
            static void emitToActor(ECS::ObjectID actorID, const T& event) {
                auto it = actorSubscribers.find(actorID);
                if (it != actorSubscribers.end()) {
                    dispatchTo(it->second, event);
                }
            }

            template<typename T>
            void emitToComponent(ComponentID componentID, const T& event) {
                auto it = componentSubscribers.find(componentID);
                if (it != componentSubscribers.end()) {
                    dispatchTo(it->second, event);
                }
            }

                
        private:
            EventDispatcher() = default;
            ~EventDispatcher() = default;
            EventDispatcher(const EventDispatcher&) = delete;
            EventDispatcher& operator=(const EventDispatcher&) = delete;

            using Handler = std::function<void(const Event&)>;
            using SubscriberMap = std::unordered_map<std::type_index, std::vector<Handler>>;

            template<typename T>
            void dispatchTo(SubscriberMap& map, const T& event) {
                auto it = map.find(typeid(T));
                if (it != map.end()) {
                    for (auto& handler : it->second) {
                        handler(event);
                    }
                }
            }

            // Subscriber storage
            static SubscriberMap globalSubscribers;
            static std::unordered_map<int, SubscriberMap> levelSubscribers;
            static std::unordered_map<ECS::ObjectID, SubscriberMap> actorSubscribers;
            static std::unordered_map<ComponentID, SubscriberMap> componentSubscribers;
    };
}