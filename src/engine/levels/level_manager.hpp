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
            
            /// @brief Load a level
            /// @param lvl The pointer to the level
            static void LoadLevel(std::shared_ptr<Level> lvl) {
                if(!lvl)
                    DEBUG_ERROR("Cannot load level (because pointer is null)");
                levelBuffer.push_back(lvl);
                lvl->Start();
            }

            /// @brief Getter for a loaded level
            /// @param index The index of the level to retrieve
            /// @return A pointer to the level loaded at "index"
            static Level* GetLevelAt(int index){
                if (index >= 0 && index < levelBuffer.size()) {
                    return levelBuffer[index].get();
                } else {
                    DEBUG_ERROR("Invalid index (out of bounds). Unable to retrieve level.");
                    return nullptr;
                }
            }

            /// @brief Unload a loaded level
            /// @param index The index of the level to unload
            static void UnLoadLevel(int index){
                if (index >= 0 && index < levelBuffer.size()) {
                    levelBuffer[index]->Clear();
                    levelBuffer.erase(levelBuffer.begin() + index);
                } else {
                    DEBUG_ERROR("Invalid index (out of bounds). Unable to unload level.");
                }
            }
        
            /// @brief Unload all loaded levels
            static void UnloadAllLevels() {
                for(int i = 0; i < levelBuffer.size(); i++){
                    levelBuffer[i]->Clear();
                }
                levelBuffer.clear();
            }
        
            
            /// @brief Getter for the total number loaded level
            /// @return The length of the level buffer
            static int GetLoadedLevelCount() {
                return levelBuffer.size();
            }

            static void Tick() {
                for(auto& level : levelBuffer){
                    level->Tick();
                }
            }
        
        private:
            LevelManager() {}  // Private constructor to prevent instantiation outside
        
            static std::vector<std::shared_ptr<Level>> levelBuffer;  // Buffer to store levels
        };
}