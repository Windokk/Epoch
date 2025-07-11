#include "resources_manager.hpp"

#include "engine/serialization/material/material_serializer.hpp"

namespace SHAME::Engine::Core::Resources{

    using namespace Filesystem;

    std::unordered_map<std::string, std::shared_ptr<Rendering::Mesh>> ResourcesManager::meshes;
    std::unordered_map<std::string, std::shared_ptr<Rendering::Texture>> ResourcesManager::textures;
    std::unordered_map<std::string, std::shared_ptr<Rendering::Shader>> ResourcesManager::shaders;
    std::unordered_map<std::string, std::shared_ptr<Rendering::Material>> ResourcesManager::materials; 

    void ResourcesManager::LoadResources(const Filesystem::Path &baseDir)
    {
        for(Filesystem::FileInfo infos : Filesystem::FileManager::ListDirectory(baseDir, {Type::T_IMAGE, Type::T_SHADER}, true)){
            switch(infos.path.GetExtensionType()){
                case Type::T_IMAGE:{
                    LoadTexture(infos.path);
                }
                case Type::T_SHADER: {
                    if(infos.path.GetParentPath().IsDirectory()){
                        std::string shaderName = infos.path.GetParent().substr(0, Filesystem::FileManager::GetRoot().full.size())+infos.name;
                        Filesystem::Path vertPath = Filesystem::Path(infos.path.GetParent()+infos.name+".vert");
                        Filesystem::Path fragPath = Filesystem::Path(infos.path.GetParent()+infos.name+".frag");
                        Filesystem::Path geomPath = Filesystem::Path(infos.path.GetParent()+infos.name+".geom");

                        std::cout<<"loading shader : "<<shaderName<<std::endl;
                        
                        if(vertPath.Exists() && fragPath.Exists()){
                            LoadShader(shaderName, vertPath, fragPath, geomPath.Exists() ? geomPath : Filesystem::Path(""));
                        }   
                        else{
                            throw std::runtime_error("[ERROR] [ENGINE/CORE/RESOURCES] : Shader loading failed. Missing .vert or .frag file in directory.");
                        }
                    }
                }
            }
        }

        for(Filesystem::FileInfo infos : Filesystem::FileManager::ListDirectory(baseDir, {Type::T_MATERIAL, Type::T_MODEL}, true)){
            switch(infos.path.GetExtensionType()){
                case Type::T_MODEL:{
                    LoadModel(infos.path);
                }
                case Type::T_MATERIAL:{
                    std::string matName = infos.path.GetParent().substr(0, Filesystem::FileManager::GetRoot().full.size())+infos.name;
                    std::cout<<"loading mat : "<<matName<<std::endl;
                    LoadMaterial(matName, infos.path);
                }
            }
        }

        for(Filesystem::FileInfo infos : Filesystem::FileManager::ListDirectory(baseDir, {Type::T_LEVEL}, true)){

        }
    }

    std::shared_ptr<Rendering::Mesh> ResourcesManager::LoadModel(const Filesystem::Path &path)
    {
        ufbx_load_opts opts = { 0 }; // Optional, pass NULL for defaults
        ufbx_error error; // Optional, pass NULL if you don't care about errors
        const std::string filePath = path.full;
        ufbx_scene *scene = ufbx_load_file(filePath.c_str(), &opts, &error);
        if (!scene) {
            throw std::runtime_error(
                "[ERROR] [ENGINE/RENDERING/MESH] : Failed to load " + path.full + " : " +
                (error.description.data ? error.description.data : "Unknown error"));
            return nullptr;
        }

        if (scene->meshes.count > 1) {
            ufbx_free_scene(scene);
            throw std::runtime_error("[ERROR] [ENGINE/RENDERING/MESH] : Multiple meshes per fbx file isn't supported yet.");
            return nullptr;
        }

        ufbx_mesh* ufbx_mesh = scene->meshes.data[0];

        std::shared_ptr<Rendering::Mesh> mesh = std::make_shared<Rendering::Mesh>(ufbx_mesh, scene->settings.unit_meters, scene->materials);
        meshes.emplace(path.full, mesh);

        ufbx_free_scene(scene);

        return mesh;

    }

    std::shared_ptr<Rendering::Texture> ResourcesManager::LoadTexture(const Filesystem::Path &path)
    {
        std::shared_ptr<Rendering::Texture> texture = std::make_shared<Rendering::Texture>(path);
        textures.emplace(path.full, texture);
        return texture;
    }

    std::shared_ptr<Rendering::Shader> ResourcesManager::LoadShader(const std::string &name, const Filesystem::Path &vsPath, const Filesystem::Path &fsPath, const Filesystem::Path &gsPath)
    {
        std::shared_ptr<Rendering::Shader> shader = std::make_shared<Rendering::Shader>(vsPath, fsPath, gsPath);
        shaders.emplace(name, shader);
        return shader;
    }

    std::shared_ptr<Rendering::Material> ResourcesManager::LoadMaterial(const std::string &name, const Filesystem::Path &path)
    {
        std::shared_ptr<Rendering::Material> mat = Serialization::MaterialSerializer::ImportMaterial(path);
        materials.emplace(name, mat);
        return mat;
    }
}