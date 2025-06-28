#include "assetID.hpp"

namespace SHAME::Engine::Filesystem
{
    std::unordered_set<int> AssetIDManager::availableIDs;
    std::map<AssetID, FileInfo*> AssetIDManager::AssetIDMap;
}