#include "audioID.hpp"

namespace EPOCH::Engine::Audio
{
    std::unordered_set<int> AudioIDManager::availableIDs;
    std::map<AudioID, Sound*> AudioIDManager::AudioIDMap;
}
