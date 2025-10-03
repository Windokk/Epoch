#pragma once

#include <string>
#include <atomic>
#include <unordered_set>
#include <map>

namespace Epoch::Engine::Filesystem
{

    class AssetID {
        public:
            
            AssetID() : packed(0) {}
            explicit AssetID(int value) : packed(value) {}

            
            int GetAsInt() const {
                return packed;
            }

            std::string GetAsString() const {
                return std::to_string(GetAsInt());
            }

            
            bool operator==(const AssetID& other) const { return packed == other.packed; }
            bool operator!=(const AssetID& other) const { return !(*this == other); }
            bool operator<(const AssetID& other) const { return packed < other.packed; }

            
            friend class AssetIDBuilder;
    
        private:
            int packed;
    };
    
    class AssetIDBuilder {
        public:
        AssetIDBuilder& WithValue(int val) {
                value = val;
                generated = false;
                return *this;
            }
        
            AssetIDBuilder& Generate() {
                value = GenerateNextID();
                generated = true;
                return *this;
            }
        
            AssetID Build() const {
                return AssetID(value);
            }
        
        private:
            static int GenerateNextID() {
                static std::atomic<int> nextId{1};
                return nextId.fetch_add(1);
            }
        
            int value = 0;
            bool generated = false;
    };

    struct FileInfo; 

    class AssetIDManager {
        public:

            static void DestroyID(const AssetID& id) {
                availableIDs.insert(id.GetAsInt());
                AssetIDMap.erase(id);
            }
            
            static AssetID GenerateNewID() {
                if (!availableIDs.empty()) {
                    int id = *availableIDs.begin();
                    availableIDs.erase(availableIDs.begin());
                    return AssetID(id);
                }
                return AssetID(AssetIDBuilder().Generate().Build().GetAsInt());
            }

            static void AssignID(AssetID id, FileInfo* info){
                AssetIDMap[id] = info;
            }
        
            static FileInfo* GetFileFromID(AssetID id){
                auto it = AssetIDMap.find(id);
                if (it != AssetIDMap.end()) {
                    return it->second;
                }
                return nullptr;
            }

            static AssetID GetIDFromString(const std::string& str){
                for (const auto& [id, info] : AssetIDMap)
                {
                    if (id.GetAsString() == str)
                        return id;
                }

                return AssetID{};
            }

        private:
            static std::map<AssetID, FileInfo*> AssetIDMap;
            static std::unordered_set<int> availableIDs;
    };
}
