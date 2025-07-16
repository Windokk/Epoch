#include "resources_manager.hpp"

#include "engine/serialization/material/material_serializer.hpp"

#include "engine/serialization/level/level_serializer.hpp"

namespace SHAME::Engine::Core::Resources{

    using namespace Filesystem;

    std::unordered_map<std::string, std::shared_ptr<Rendering::Mesh>> ResourcesManager::meshes;
    std::unordered_map<std::string, std::shared_ptr<Rendering::Texture>> ResourcesManager::textures;
    std::unordered_map<std::string, std::shared_ptr<Rendering::Shader>> ResourcesManager::shaders;
    std::unordered_map<std::string, std::shared_ptr<Rendering::Material>> ResourcesManager::materials; 
    std::unordered_map<std::string, std::shared_ptr<Levels::Level>> ResourcesManager::levels; 

    void ResourcesManager::LoadResources(const Filesystem::Path &baseDir)
    {
        std::cout<<"[INFO] [ENGINE/CORE/RESOURCES] : Loading resources from base directory: "<<baseDir.full<<std::endl;

        if(!baseDir.Exists() || !baseDir.IsDirectory()){
            throw std::runtime_error("[ERROR] [ENGINE/CORE/RESOURCES] : Base directory for resources does not exist or is not a directory.");
        }
        for(Filesystem::FileInfo infos : Filesystem::FileManager::ListDirectory(baseDir, {Type::T_IMAGE, Type::T_SHADER}, false, true)){
            switch(infos.path.GetExtensionType()){
                case Type::T_IMAGE:{
                    std::string textureName = infos.path.RelativeTo(baseDir).full;
                    if (textures.find(textureName) != textures.end()) {
                        break;
                    }
                    std::cout<<"[INFO] [ENGINE/CORE/RESOURCES] : Loading texture : "<<textureName<<std::endl;
                    LoadTexture(textureName, infos.path);
                    break;
                }
                case Type::T_SHADER: {
                    if(infos.path.GetParentPath().IsDirectory()){

                        std::string shaderName = infos.path.RelativeTo(baseDir).WithoutExtension();

                        if (shaders.find(shaderName) != shaders.end()) {
                            break;
                        }

                        Filesystem::Path vertPath = Filesystem::Path(
                                                        (std::filesystem::path(infos.path.GetParent()) / (infos.name + ".vert")).string()
                                                    );
                        Filesystem::Path fragPath = Filesystem::Path(
                                                        (std::filesystem::path(infos.path.GetParent()) / (infos.name + ".frag")).string()
                                                    );
                        Filesystem::Path geomPath = Filesystem::Path(
                                                        (std::filesystem::path(infos.path.GetParent()) / (infos.name + ".geom")).string()
                                                    );

                        
                        if(vertPath.Exists() && fragPath.Exists()){
                            std::cout<<"[INFO] [ENGINE/CORE/RESOURCES] : Loading shader : "<<shaderName<<std::endl;

                            LoadShader(shaderName, vertPath, fragPath, geomPath.Exists() ? geomPath : Filesystem::Path(""));
                        }   
                        else{
                            throw std::runtime_error("[ERROR] [ENGINE/CORE/RESOURCES] : Shader loading failed. Missing .vert or .frag file in directory.");
                        }
                    }
                    break;
                }
            }
        }

        for(Filesystem::FileInfo infos : Filesystem::FileManager::ListDirectory(baseDir, {Type::T_MATERIAL, Type::T_MODEL}, false, true)){
            switch(infos.path.GetExtensionType()){
                case Type::T_MODEL:{
                    std::string meshName = infos.path.RelativeTo(baseDir).full;
                    if (meshes.find(meshName) != meshes.end()) {
                        break;
                    }
                    std::cout<<"[INFO] [ENGINE/CORE/RESOURCES] : Loading model : "<<meshName<<std::endl;
                    LoadModel(meshName, infos.path);
                    break;
                }
                case Type::T_MATERIAL:{
                    std::string matName = infos.path.RelativeTo(baseDir).WithoutExtension();
                    if (materials.find(matName) != materials.end()) {
                        break;
                    }
                    std::cout<<"[INFO] [ENGINE/CORE/RESOURCES] : Loading material : "<<matName<<std::endl;
                    LoadMaterial(matName, infos.path);
                    break;
                }
            }
        }
    
        for(Filesystem::FileInfo infos : Filesystem::FileManager::ListDirectory(baseDir, {Type::T_LEVEL}, false, true)){
            std::string levelName = infos.path.RelativeTo(baseDir).full;
            if (meshes.find(levelName) != meshes.end()) {
                break;
            }
            std::cout<<"[INFO] [ENGINE/CORE/RESOURCES] : Loading level : "<<levelName<<std::endl;
            LoadLevel(levelName, infos.path);
            break;
        }
    }

    std::shared_ptr<Rendering::Mesh> ResourcesManager::LoadModel(const std::string &name, const Filesystem::Path &path)
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
            throw std::runtime_error("[ERROR] [ENGINE/CORE/RESOURCES/MODEL] : Multiple meshes per fbx file isn't supported yet.");
            return nullptr;
        }

        ufbx_mesh* ufbx_mesh = scene->meshes.data[0];

        std::shared_ptr<Rendering::Mesh> mesh = std::make_shared<Rendering::Mesh>(ufbx_mesh, scene->settings.unit_meters, scene->materials);
        meshes.emplace(name, mesh);

        ufbx_free_scene(scene);

        return mesh;

    }

    std::shared_ptr<Rendering::Texture> ResourcesManager::LoadTexture(const std::string &name, const Filesystem::Path &path)
    {
        std::shared_ptr<Rendering::Texture> texture = std::make_shared<Rendering::Texture>(path);
        textures.emplace(name, texture);
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
        if(!mat){
            throw std::runtime_error("[ERROR] [ENGINE/CORE/RESOURCES/MATERIAL] : Unknown error during material import.");
        }
        materials.emplace(name, mat);
        return mat;
    }

    std::shared_ptr<Levels::Level> ResourcesManager::LoadLevel(const std::string &name, const Filesystem::Path& path){
        std::shared_ptr<Levels::Level> level = Serialization::LevelSerializer::ImportLevel(path);
        if(!level){
            throw std::runtime_error("[ERROR] [ENGINE/CORE/RESOURCES/LEVEL] : Unknown error during level import.");
        }
        auto [it, inserted] = levels.emplace(name, level);
        if (!inserted) {
            throw std::runtime_error("[WARNING] [ENGINE/CORE/RESOURCES/LEVEL] : Level '" + name + "' already loaded.");
        }
        return level;
    }
}