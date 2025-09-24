#include "event_system.hpp"

namespace EPOCH::Engine::Events{

    using Handler = std::function<void(const Event&)>;
    using SubscriberMap = std::unordered_map<std::type_index, std::vector<Handler>>;

    SubscriberMap EventDispatcher::globalSubscribers;
    std::unordered_map<int, SubscriberMap> EventDispatcher::levelSubscribers;
    std::unordered_map<ECS::ObjectID, SubscriberMap> EventDispatcher::actorSubscribers;
    std::unordered_map<uint32_t, SubscriberMap> EventDispatcher::componentSubscribers;
}