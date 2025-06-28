#include "audioID.hpp"

namespace SHAME::Engine::Audio
{
    std::unordered_set<int> AudioIDManager::availableIDs;
    std::map<AudioID, Sound*> AudioIDManager::AudioIDMap;
}
