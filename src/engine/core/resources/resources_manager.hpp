#pragma once

#include <unordered_map>

#include "engine/filesystem/filesystem.hpp"
#include "engine/rendering/mesh/mesh.hpp"
#include "engine/levels/level.hpp"

namespace SHAME::Engine::Core::Resources{

    class ResourcesManager{
        public:
            static ResourcesManager& GetInstance() {
                static ResourcesManager instance;
                return instance;
            }
            static void LoadResources(const Filesystem::Path& baseDir);
            static std::shared_ptr<Rendering::Mesh> LoadModel(const std::string &name, const Filesystem::Path &path);
            static std::shared_ptr<Rendering::Texture> LoadTexture(const std::string &name, const Filesystem::Path &path);
            static std::shared_ptr<Rendering::Shader> LoadShader(const std::string &name, const Filesystem::Path &vsPath, const Filesystem::Path &fsPath, const Filesystem::Path &gsPath);
            static std::shared_ptr<Rendering::Material> LoadMaterial(const std::string &name, const Filesystem::Path &path);
            static std::shared_ptr<Levels::Level> LoadLevel(const std::string &name, const Filesystem::Path& path);

            static std::shared_ptr<Rendering::Mesh> GetMesh(std::string name) { 
                auto it = meshes.find(name);
                if (it != meshes.end())
                    return it->second;
                return nullptr;
            }

            static std::shared_ptr<Rendering::Material> GetMaterial(std::string name) { 
                auto it = materials.find(name);
                if (it != materials.end())
                    return it->second;
                return nullptr;
            }

            static std::shared_ptr<Rendering::Shader> GetShader(std::string name) { 
                auto it = shaders.find(name);
                if (it != shaders.end())
                    return it->second;
                return nullptr;
            }

            static std::shared_ptr<Rendering::Texture> GetTexture(std::string name) { 
                auto it = textures.find(name);
                if (it != textures.end())
                    return it->second;
                return nullptr;
            }

            static std::shared_ptr<Levels::Level> GetLevel(std::string name){
                auto it = levels.find(name);
                if (it != levels.end())
                    return it->second;
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
            static std::unordered_map<std::string, std::shared_ptr<Levels::Level>> levels;
        };

}