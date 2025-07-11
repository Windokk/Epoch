#include "objectID.hpp"

namespace SHAME::Engine::ECS
{
    std::unordered_set<int> ObjectIDManager::availableIDs;
    std::map<ObjectID, std::shared_ptr<Objects::Object>> ObjectIDManager::ObjectIDMap;
}
