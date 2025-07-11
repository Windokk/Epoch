#pragma once

#include <unordered_map>

#include "engine/filesystem/filesystem.hpp"
#include "engine/rendering/mesh/mesh.hpp"

namespace SHAME::Engine::Core::Resources{

    class ResourcesManager{
        public:
            static ResourcesManager& GetInstance() {
                static ResourcesManager instance;
                return instance;
            }
            static void LoadResources(const Filesystem::Path& baseDir);
            static std::shared_ptr<Rendering::Mesh> LoadModel(const Filesystem::Path& path);
            static std::shared_ptr<Rendering::Texture> LoadTexture(const Filesystem::Path& path);
            static std::shared_ptr<Rendering::Shader> LoadShader(const std::string &name, const Filesystem::Path &vsPath, const Filesystem::Path &fsPath, const Filesystem::Path &gsPath);
            static std::shared_ptr<Rendering::Material> LoadMaterial(const std::string &name, const Filesystem::Path &path);

            static std::shared_ptr<Rendering::Mesh> GetMesh(std::string name) { 
                if(meshes[name]) 
                    return meshes[name]; 
                return nullptr;
            }

            static std::shared_ptr<Rendering::Material> GetMaterial(std::string name) { 
                if(materials[name]) 
                    return materials[name]; 
                return nullptr;
            }

            static std::shared_ptr<Rendering::Shader> GetShader(std::string name) { 
                if(shaders[name]) 
                    return shaders[name];
                return nullptr;
            }

            static std::shared_ptr<Rendering::Texture> GetTexture(std::string name) { 
                if(textures[name]) 
                    return textures[name]; 
                return nullptr;
            }

            //TODO static void UnloadUnused();
            static void Clear();

        private:
            ResourcesManager() = default;
            ~ResourcesManager() = default;
            ResourcesManager(const ResourcesManager&) = delete;
            ResourcesManager& operator=(const ResourcesManager&) = delete;

            static std::unordered_map<std::string, std::shared_ptr<Rendering::Mesh>> meshes;
            static std::unordered_map<std::string, std::shared_ptr<Rendering::Texture>> textures;
            static std::unordered_map<std::string, std::shared_ptr<Rendering::Shader>> shaders;
            static std::unordered_map<std::string, std::shared_ptr<Rendering::Material>> materials;  
    };

}