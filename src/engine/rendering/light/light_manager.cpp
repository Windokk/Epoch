#include "light_manager.hpp"
#include <type_traits>
#include <iostream>

namespace SHAME::Engine::Rendering{
    
    LightManager::LightManager()
    {
        glGenBuffers(1, &ssbo);
    }

    LightManager::~LightManager()
    {
        glDeleteBuffers(1, &ssbo);
    }

    void LightManager::Update(){

        if(lights.size() != 0){
            std::vector<GPULight> flatLights;
            flatLights.reserve(lights.size());

            for (std::shared_ptr<GPULight> light : lights) {
                if (light) {
                    flatLights.push_back(*light.get());
                }
            }

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GPULight) * flatLights.size(), flatLights.data(), GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
        }
    }

    void LightManager::AddLight(std::shared_ptr<GPULight> light)
    {
        lights.push_back(light);
    }

    void LightManager::Clear(){
        lights.clear();
    }

    int LightManager::GetLightsCount()
    {
        return lights.size();
    }
}