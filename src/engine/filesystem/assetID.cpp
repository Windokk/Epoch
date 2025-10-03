#include "assetID.hpp"

namespace Epoch::Engine::Filesystem
{
    std::unordered_set<int> AssetIDManager::availableIDs;
    std::map<AssetID, FileInfo*> AssetIDManager::AssetIDMap;
}