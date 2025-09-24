#include "camera_manager.hpp"

namespace EPOCH::Engine::Rendering{

    std::unordered_map<std::string, std::shared_ptr<Camera>> CameraManager::cameras;
    std::shared_ptr<Camera> CameraManager::activeCamera = nullptr;
}