#pragma once

#include "level.hpp"

namespace SHAME::Engine::Levels{

    class LevelManager {
        public:
            static LevelManager& GetInstance() {
                static LevelManager instance;
                return instance;
            }
        
            LevelManager(const LevelManager&) = delete;
            LevelManager& operator=(const LevelManager&) = delete;
            
            static void LoadLevel(std::unique_ptr<Level> lvl) {
                levelBuffer.push_back(std::move(lvl));
            }

            static Level* GetLevelAt(int index){
                if (index >= 0 && index < levelBuffer.size()) {
                    return levelBuffer[index].get();
                } else {
                    throw std::runtime_error("[ERROR] [LEVELS] Invalid index. Unable to unload level.");
                }
            }

            static void UnLoadLevel(int index){
                if (index >= 0 && index < levelBuffer.size()) {
                    levelBuffer.erase(levelBuffer.begin() + index);
                } else {
                    throw std::runtime_error("[ERROR] [LEVELS] Invalid index. Unable to unload level.");
                }
            }
        
            static void UnloadAllLevels() {
                levelBuffer.clear();
            }
        
            
            static int GetLoadedLevelCount() {
                return levelBuffer.size();
            }
        
        private:
            LevelManager() {}  // Private constructor to prevent instantiation outside
        
            static std::vector<std::unique_ptr<Level>> levelBuffer;  // Buffer to store levels
        };
}