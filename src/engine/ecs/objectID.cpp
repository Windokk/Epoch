#include "objectID.hpp"

namespace SHAME::Engine::ECS
{
    std::unordered_set<int> ObjectIDManager::availableIDs;
    std::map<ObjectID, Objects::Object*> ObjectIDManager::ObjectIDMap;
}
