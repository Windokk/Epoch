#pragma once

#include "level.hpp"

namespace Epoch::Engine::Levels{

    class LevelManager {
        public:
            static LevelManager& GetInstance() {
                static LevelManager instance;
                return instance;
            }
        
            LevelManager() = default;
            ~LevelManager() = default;
            LevelManager(const LevelManager&) = delete;
            LevelManager& operator=(const LevelManager&) = delete;
            
            /// @brief Load a level
            /// @param lvl The pointer to the level
            void LoadLevel(std::shared_ptr<Level> lvl) {
                if(!lvl)
                    DEBUG_FATAL("Cannot load level (because pointer is null)");
                levelBuffer.push_back(lvl);
                lvl->loaded = true;
                lvl->Start();
            }

            /// @brief Getter for a loaded level
            /// @param index The index of the level to retrieve
            /// @return A pointer to the level loaded at "index"
            Level* GetLevelAt(int index){
                if (index >= 0 && index < levelBuffer.size()) {
                    return levelBuffer[index].get();
                } else {
                    DEBUG_ERROR("Invalid index (out of bounds). Unable to retrieve level.");
                    return nullptr;
                }
            }

            /// @brief Unload a loaded level
            /// @param index The index of the level to unload
            void UnloadLevel(int index){
                if (index >= 0 && index < levelBuffer.size()) {
                    levelBuffer[index]->Unload();
                    levelBuffer.erase(levelBuffer.begin() + index);
                } else {
                    DEBUG_ERROR("Invalid index (out of bounds). Unable to unload level.");
                }
            }
            
            /// @brief Unload all loaded levels
            void UnloadAllLevels() {
                for(int i = 0; i < levelBuffer.size(); i++){
                    levelBuffer[i]->Unload();
                }
                levelBuffer.clear();
            }
    
            /// @brief Destroys a level
            void DestroyLevel(std::shared_ptr<Level> level){
                level->Unload();
                level->Clear();
            }
        
            /// @brief Destroys all levels
            void DestroyAllLevels() {
                for(int i = 0; i < levelBuffer.size(); i++){
                    levelBuffer[i]->Unload();
                    levelBuffer[i]->Clear();
                }
                levelBuffer.clear();
            }
            
            /// @brief Getter for the total number loaded level
            /// @return The length of the level buffer
            int GetLoadedLevelCount() {
                return levelBuffer.size();
            }

            void Tick() {
                for(auto& level : levelBuffer){
                    level->Tick();
                }
            }
        
        private:

            std::vector<std::shared_ptr<Level>> levelBuffer;  // Buffer to store levels
        };
}