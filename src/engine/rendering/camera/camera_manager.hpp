#pragma once

#include "engine/rendering/utils.hpp"
#include "engine/ecs/components/rendering/camera.hpp"

#include <string>
#include <vector>
#include <memory>

namespace EPOCH::Engine::Rendering{
    
    using namespace ECS::Components;

    class CameraManager{
        public:
            static CameraManager& GetInstance() {
                static CameraManager instance;
                return instance;
            }
            
            /// @brief Adds a new camera with a name
            static void AddCamera(const std::string& name, std::shared_ptr<Camera> camera) {
                cameras[name] = camera;
                if (!activeCamera)
                    activeCamera = camera;
            }

            /// @brief Removes a camera by name
            static void RemoveCamera(const std::string& name) {
                if (cameras.count(name)) {
                    if (cameras[name] == activeCamera)
                        activeCamera = nullptr;
                    cameras.erase(name);
                }
            }

            /// @brief Update all camera's sizes
            /// @param width The new width (in px)
            /// @param height The new height (in px)
            static void UpdateSize(int width, int height) {
                for(std::pair<std::string, std::shared_ptr<Camera>> cam : cameras){
                    cam.second->UpdateSize(width, height);
                }
            }

            /// @brief Update the active camera
            static void Tick(){
                activeCamera->UpdateMatrix();
            }

            /// @brief Get a camera by name
            static std::shared_ptr<Camera> GetCamera(const std::string& name) {
                if (cameras.count(name))
                    return cameras[name];
                return nullptr;
            }

            /// @brief Set the active camera
            static void SetActiveCamera(const std::string& name) {
                if (cameras.count(name))
                    activeCamera = cameras[name];
            }

            /// @brief Get the current active camera
            static std::shared_ptr<Camera> GetActiveCamera() {
                return activeCamera;
            }

            /// @brief Clear all cameras
            static void Clear() {
                cameras.clear();
                activeCamera = nullptr;
            }

        private:
            CameraManager() = default;
            ~CameraManager() = default;
            CameraManager(const CameraManager&) = delete;
            CameraManager& operator=(const CameraManager&) = delete;

            static std::unordered_map<std::string, std::shared_ptr<Camera>> cameras;
            static std::shared_ptr<Camera> activeCamera;
    };
}