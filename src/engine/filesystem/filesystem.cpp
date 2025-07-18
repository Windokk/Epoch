#include "filesystem.hpp"

#include "packer/unpacker.hpp"

#include "engine/debugging/debugger.hpp"

namespace SHAME::Engine::Filesystem{

    Path FileManager::root = Path("");
    Path FileManager::projectRoot = Path("");

    void FileManager::Init(std::string rootPath, std::string projectRootPath)
    {
        if(rootPath == ""){
            root = Path(std::filesystem::current_path().string());
        }

        if(projectRootPath == ""){
            projectRoot = Path(root.full+"\\project_resources\\");
        }
    }


    std::vector<FileInfo> FileManager::ListDirectory(const Path &path, std::vector<Type> acceptedExtensions, bool includeDirs, bool recursive)
    {
        std::vector<FileInfo> files;

        if (!std::filesystem::exists(path.full) || !std::filesystem::is_directory(path.full))
            DEBUG_ERROR("Cannot list files inside non-existing directory");

        auto processEntry = [&](const auto& entry) {
            Path filePath(entry.path().string());
            
            if (entry.is_directory() && includeDirs)
            {
                FileInfo info = GetFileInfos(filePath);
                files.push_back(info);
            }
            else if (entry.is_regular_file())
            {
                auto extType = filePath.GetExtensionType();
                if (acceptedExtensions.empty() ||
                    std::find(acceptedExtensions.begin(), acceptedExtensions.end(), extType) != acceptedExtensions.end()) {
                    files.push_back(GetFileInfos(filePath));
                }
            }
        };

        if (recursive)
        {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(path.full,
                        std::filesystem::directory_options::skip_permission_denied))
            {
                processEntry(entry);
            }
        }
        else
        {
            for (const auto& entry : std::filesystem::directory_iterator(path.full,
                        std::filesystem::directory_options::skip_permission_denied))
            {
                processEntry(entry);
            }
        }

        return files;
    }

    FileInfo FileManager::GetFileInfos(const Path &path)
    {
        FileInfo infos;
    
        infos.path = path.full;
        infos.name = path.GetFilename(false);
        infos.isDirectory = path.IsDirectory();
        infos.size = path.GetFileSize();
        infos.type = path.GetExtensionType();

        return infos;
    }

    std::string Path::ReadFile() const
    {
        if(IsPacked()){
            CosemUnPacker unpacker;
            unpacker.OpenArchive(GetParentArchive());
            std::vector<char> characters = unpacker.ExtractFileToMemory(GetPathInsideArchive());
            return std::string(characters.begin(), characters.end());
        }
        else{
            std::ifstream file(full, std::ios::binary);
            if (!file) DEBUG_ERROR("Can't read file at path : " + full);
            std::ostringstream sstream;
            sstream << file.rdbuf();
            return sstream.str();
        }
    }

    std::string Path::WithoutExtension() const
    {
        std::filesystem::path p(full);
        return (p.parent_path() / p.stem()).string();
    }

    bool Path::WriteFile(const std::string &content) const
    {
        if (IsPacked()) {
            DEBUG_ERROR("Cannot write to a file inside a .caf archive");
            return false;
        }
    
        std::ofstream out(full, std::ios::trunc); // truncate = overwrite if file exists
        if (!out.is_open()) return false;
    
        try {
            out << content;
        } catch (...) {
            return false;
        }
    
        return true;
    }

    bool Path::AppendToFile(const std::string &content) const
    {
        if(IsPacked()){
            DEBUG_ERROR("Cannot create append to file placed in .caf archive");
            return false;
        }

        std::ofstream out(full, std::ios::app);
        if (!out.is_open()) return false;
        try {
            out << content;
        } catch (...) {
            return false;
        }
        return true;
    }

    Type Path::GetExtensionType() const
    {
        std::string str = std::filesystem::path(GetAbsolutePath()).extension().string();

        std::transform(str.begin(), str.end(), str.begin(), ::tolower);

        static const std::unordered_map<std::string, Type> extensionMap = {
            // Image formats
            {".png", Type::T_IMAGE}, {".jpg", Type::T_IMAGE}, {".jpeg", Type::T_IMAGE},
            {".bmp", Type::T_IMAGE}, {".gif", Type::T_IMAGE}, {".tga", Type::T_IMAGE},
            // Sound formats
            {".wav", Type::T_SOUND}, {".mp3", Type::T_SOUND}, {".ogg", Type::T_SOUND},
            // Font formats
            {".ttf", Type::T_FONT}, {".otf", Type::T_FONT}, {".fnt", Type::T_FONT},
            // Shader formats
            {".geom", Type::T_SHADER}, {".frag", Type::T_SHADER}, {".vert", Type::T_SHADER},
            // Text formats
            {".txt", Type::T_TEXT}, {".md", Type::T_TEXT},
            // Script formats
            {".cpp", Type::T_SCRIPT}, {".hpp", Type::T_SCRIPT},
            // Model formats
            {".fbx", Type::T_MODEL},
            // Material formats
            {".mat", Type::T_MATERIAL}, {".material", Type::T_MATERIAL},
            // Level formats
            {".level", Type::T_LEVEL}, {".lvl", Type::T_LEVEL},
            // Config formats
            {".json", Type::T_CONFIG}, {".ini", Type::T_CONFIG}, {".cfg", Type::T_CONFIG},
            // Directory (fallback, not based on extension)
            {"<directory>", Type::T_DIRECTORY}  // special case if needed
        };

        // Ensure extension starts with dot
        if (str.empty() || str[0] != '.') {
            str = "." + str;
        }

        auto it = extensionMap.find(str);
        if (it != extensionMap.end()) {
            return it->second;
        }

        return Type::T_TEXT;
    }

    bool FileManager::IsPathInside(const Path &parent, const Path &child)
    {
        std::filesystem::path parentPath = std::filesystem::weakly_canonical(parent.GetAbsolutePath());
        std::filesystem::path childPath = std::filesystem::weakly_canonical(child.GetAbsolutePath());

        // Check if childPath starts with parentPath
        auto parentIt = parentPath.begin();
        auto childIt = childPath.begin();

        for (; parentIt != parentPath.end(); ++parentIt, ++childIt) {
            if (childIt == childPath.end() || *parentIt != *childIt)
                return false;
        }
        return true;
    }

    bool FileManager::HasExtension(const Path &path, const std::vector<std::string> &validExtensions)
    {
        std::string ext = path.GetExtensionString();
    
        // Normalize the extension: lowercase and strip leading dot if present
        if (!ext.empty() && ext[0] == '.')
            ext = ext.substr(1);
        
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        for (const auto& valid : validExtensions) {
            std::string validExt = valid;
            if (!validExt.empty() && validExt[0] == '.')
                validExt = validExt.substr(1);
            std::transform(validExt.begin(), validExt.end(), validExt.begin(), ::tolower);

            if (ext == validExt)
                return true;
        }
        return false;
    }

    void FileManager::SetRoot(const Path &path)
    {
        root = path;
    }

    void FileManager::SetProjectRoot(const Path &path)
    {
        projectRoot = path;
    }

    Path::Path(const std::string &raw)
    {
        full = Normalize(raw);
    }

    std::string Path::Normalize(const std::string &raw)
    {
        return std::filesystem::weakly_canonical(raw).string();
    }

    Path Path::RelativeTo(const Path &other) const
    {
        try {
            std::filesystem::path rel = std::filesystem::relative(full, other.full);
            return Path(rel.string());
        } catch (const std::filesystem::filesystem_error& e) {
            return *this;
        }
    }

    std::string Path::GetExtensionString() const
    {
        auto ext = std::filesystem::path(full).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }

    std::string Path::GetAbsolutePath() const
    {
        std::filesystem::path abs = std::filesystem::weakly_canonical(std::filesystem::absolute(full));
        return abs.string();
    }

    std::string Path::GetFilename(bool withExtension) const
    {
        auto p = std::filesystem::path(full);
        return withExtension ? p.filename().string() : p.stem().string();
    }

    std::string Path::GetParent() const
    {
        return std::filesystem::path(full).parent_path().string();
    }

    int Path::GetFileSize() const
    {
        if (IsPacked()) {
            CosemUnPacker unpacker;
            unpacker.OpenArchive(GetParentArchive());
            auto fileIndex = unpacker.GetFileIndex();

            std::string insidePath = GetPathInsideArchive();

            if (IsDirectory() && !insidePath.empty() && insidePath.back() != '/')
                insidePath += '/';

            int totalSize = 0;

            for (const auto& entry : fileIndex) {
                if (entry.path == insidePath || entry.path.compare(0, insidePath.size(), insidePath) == 0) {
                    totalSize += entry.fileSize;
                }
            }

            return static_cast<int>(totalSize);
        } else {
            if (IsDirectory()) {
                int totalSize = 0;

                for (const auto& entry : std::filesystem::recursive_directory_iterator(full,
                            std::filesystem::directory_options::skip_permission_denied))
                {
                    if (entry.is_regular_file()) {
                        totalSize += std::filesystem::file_size(entry.path());
                    }
                }

                return static_cast<int>(totalSize);
            } else if (std::filesystem::exists(full) && std::filesystem::is_regular_file(full)) {
                return static_cast<int>(std::filesystem::file_size(full));
            } else {
                DEBUG_ERROR("File or directory does not exist: " + full);
                return -1;
            }
        }
    }

    Path Path::GetParentPath() const
    {
        return Path(GetParent());
    }

    std::string Path::GetParentArchive() const
    {
        if(!IsPacked()){
            DEBUG_ERROR("Cannot get parent archive for a file that is not placed in an archive");
            return "";
        }
        else{

            size_t pos = full.find(".caf");
            if (pos == std::string::npos) {
                DEBUG_ERROR("Packed resource path does not contain '.caf', it is not placed in an archive");
            }

            return full.substr(0, pos + 4);
        }
    }

    std::string Path::GetPathInsideArchive() const
    {
        const std::string &fullPath = this->full;
        size_t pos = fullPath.find(".caf");
        if (pos == std::string::npos) {
            DEBUG_ERROR("File is not inside an archive");
        }

        size_t start = pos + 5;
        if (start >= fullPath.size()) return "";
    
        return fullPath.substr(start);
    }

    bool Path::IsPacked() const
    {
        return full.find(".caf/") != std::string::npos;
    }

    bool Path::Exists() const
    {
        return std::filesystem::exists(full);
    }

    bool Path::IsDirectory() const
    {
        return std::filesystem::is_directory(full);
    }

    bool Path::IsSubPathOf(const Path &child, const Path &parent)
    {
        auto childStr = std::filesystem::weakly_canonical(child.full).string();
        auto parentStr = std::filesystem::weakly_canonical(parent.full).string();
    
        // Ensure parent path ends with a slash to avoid partial match
        if (parentStr.back() != '/' && parentStr.back() != '\\')
            parentStr += '/';
    
        return childStr.compare(0, parentStr.size(), parentStr) == 0;
    }
}