#include "objectID.hpp"

namespace EPOCH::Engine::ECS
{
    std::unordered_set<int> ObjectIDManager::availableIDs;
    std::map<ObjectID, std::shared_ptr<Objects::Object>> ObjectIDManager::ObjectIDMap;
}
