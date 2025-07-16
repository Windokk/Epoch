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
        T_LEVEL,
        T_MODEL,
        T_MATERIAL,
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
    
        Path RelativeTo(const Path& other) const;

        std::string GetExtensionString() const;

        std::string ReadFile() const;

        std::string WithoutExtension() const;

        bool WriteFile(const std::string &content) const;

        bool AppendToFile(const std::string &content) const;

        Type GetExtensionType() const;

        std::string GetAbsolutePath() const;

        std::string GetFilename(bool withExtension = true) const;
    
        std::string GetParent() const;

        int GetFileSize() const;

        Path GetParentPath() const;
    
        std::string GetParentArchive() const ;

        std::string GetPathInsideArchive() const;

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
        int size;
    };

    class FileManager {
        public:
            static FileManager& GetInstance() {
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
            static std::vector<FileInfo> ListDirectory(const Path &path, std::vector<Type> acceptedExtensions, bool includeDirs = false, bool recursive = false);

            // Path tools
            static bool IsPathInside(const Path& parent, const Path& child);
            static bool HasExtension(const Path& path, const std::vector<std::string>& validExtensions);

            // Utility
            static void SetRoot(const Path& path);
            static Path GetRoot() { return root; };

        private:
            static Path root;
            FileManager() = default;
            ~FileManager() = default;
    };
}