#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <algorithm>
#include <fstream>

#include <nlohmann/json.hpp>

#include "assetID.hpp"

namespace SHAME::Engine::Filesystem{

    enum class Type{
        T_IMAGE,
        T_SOUND,
        T_FONT,
        T_SHADER,
        T_TEXT,
        T_SCRIPT,
        T_ANIMATION,
        T_SCENE,
        T_CONFIG,
        T_DIRECTORY
    };

    // --- Path infos ---
    class Path {

        public:
        std::string full;  // Full/normalized path
    
        Path() = default;
        Path(const std::string& raw);
    
        // Normalize slashes, remove ../, etc.
        static std::string Normalize(const std::string& raw);
    
        std::string GetExtensionString() const;

        std::string GetAbsolutePath() const;

        std::string Filename(bool withExtension = true) const;
    
        std::string Parent() const;
    
        std::string GetParentArchive() const ;

        std::string GetPathInsideArchive() const;
    
        std::string ResourcePath() const;

        bool IsPacked() const;
    
        bool Exists() const;
    
        bool IsDirectory() const;

        static bool IsSubPathOf(const Path& child, const Path& parent);
    };

    // --- Basic file info ---
    struct FileInfo {
        Path path;
        std::string name;
        Type type;
        bool isDirectory;
        bool hasCosem;
        int size;
    };

    class FileManager {
        public:
            static FileManager& getInstance() {
                static FileManager instance;
                return instance;
            }

            FileManager(const FileManager&) = delete;
            FileManager& operator=(const FileManager&) = delete;
            FileManager(FileManager&&) = delete;
            FileManager& operator=(FileManager&&) = delete;
            
            static void Init(std::string rootPath = "");

            // File and directory browsing
            static FileInfo GetFileInfos(const Path& path);
            static std::vector<FileInfo> ListDirectory(const Path &path, bool recursive = false);
            static bool Exists(const Path& path);
            static bool IsDirectory(const Path& path);

            // File content
            static int GetFileSize(const Path& path);
            static std::string ReadFile(const Path& path);
            static bool WriteFile(const Path& path, const std::string& content);
            static bool AppendToFile(const Path& path, const std::string& content);

            // Extension and type utilities
            static std::string GetExtension(const Path& path);
            static std::string GetTypeString(const Type &type);
            static Type GetTypeFromExtensionString(std::string str);
            static std::string GetFileName(const Path &path, bool withExtension = true);
            static std::string GetParentPath(const Path& path);

            // Path tools
            static bool IsPathInside(const Path& parent, const Path& child);
            static bool HasExtension(const Path& path, const std::vector<std::string>& validExtensions);

            

            // Utility
            static void SetRoot(const Path& path);
            static bool IsPackedResource(const Path& path);
        

        private:
            static Path root;
            FileManager() = default;
            ~FileManager() = default;
    };
}