#pragma once

#include <unordered_map>

#include "engine/filesystem/filesystem.hpp"
#include "engine/rendering/mesh/mesh.hpp"
#include "engine/levels/level.hpp"

namespace EPOCH::Engine::Core::Resources{

    class ResourcesManager{
        public:
            static ResourcesManager& GetInstance() {
                static ResourcesManager instance;
                return instance;
            }
            void LoadResourcesInDir(const Filesystem::Path &baseDir);
            void LoadResources(const Filesystem::Path &projectDir, const Filesystem::Path &engineDir);
            std::shared_ptr<Rendering::Mesh> LoadModel(const std::string &name, const Filesystem::Path &path);
            std::shared_ptr<Rendering::Texture> LoadTexture(const std::string &name, const Filesystem::Path &path);
            std::shared_ptr<Rendering::Shader> LoadShader(const std::string &name, const Filesystem::Path &vsPath, const Filesystem::Path &fsPath, const Filesystem::Path &gsPath);
            std::shared_ptr<Rendering::Material> LoadMaterial(const std::string &name, const Filesystem::Path &path);
            std::shared_ptr<Levels::Level> LoadLevel(const std::string &name, const Filesystem::Path& path);

            std::shared_ptr<Rendering::Mesh> GetMesh(std::string name) { 
                auto it = meshes.find(name);
                if (it != meshes.end())
                    return it->second;
                return nullptr;
            }

            std::shared_ptr<Rendering::Material> GetMaterial(std::string name) { 
                auto it = materials.find(name);
                if (it != materials.end())
                    return it->second;
                return nullptr;
            }

            std::shared_ptr<Rendering::Shader> GetShader(std::string name) { 
                auto it = shaders.find(name);
                if (it != shaders.end())
                    return it->second;
                return nullptr;
            }

            std::shared_ptr<Rendering::Texture> GetTexture(std::string name) { 
                auto it = textures.find(name);
                if (it != textures.end())
                    return it->second;
                return nullptr;
            }

            std::shared_ptr<Levels::Level> GetLevel(const std::string& name) {
                if (auto it = levels.find(name); it != levels.end())
                    return it->second;
                return nullptr;
            }

            //TODO static void UnloadUnused();
            void Clear();

        private:
            ResourcesManager() = default;
            ~ResourcesManager() = default;
            ResourcesManager(const ResourcesManager&) = delete;
            ResourcesManager& operator=(const ResourcesManager&) = delete;

            std::unordered_map<std::string, std::shared_ptr<Rendering::Mesh>> meshes;
            std::unordered_map<std::string, std::shared_ptr<Rendering::Texture>> textures;
            std::unordered_map<std::string, std::shared_ptr<Rendering::Shader>> shaders;
            std::unordered_map<std::string, std::shared_ptr<Rendering::Material>> materials;
            std::unordered_map<std::string, std::shared_ptr<Levels::Level>> levels;
        };

}