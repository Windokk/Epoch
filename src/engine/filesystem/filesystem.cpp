#include "filesystem.hpp"

#include "packer/unpacker.hpp"

namespace SHAME::Engine::Filesystem{

    Path FileManager::root = Path("");

    void FileManager::InitializeSession(std::string rootPath)
    {
        if(rootPath == ""){
            root = Path(std::filesystem::current_path().string());
        }
    }


    std::vector<FileInfo> FileManager::ListDirectory(const Path &path, bool recursive)
    {
        std::vector<FileInfo> files;

        if (!std::filesystem::exists(path.full) || !std::filesystem::is_directory(path.full))
            throw std::runtime_error("[ERROR]  [ENGINE/FILESYSTEM] : Cannot list files insiide non-existing directory");

        auto processEntry = [&](const auto& entry) {
            const auto& fsPath = entry.path();
            
            if (entry.is_directory())
            {
                FileInfo info = GetFileInfos(Path(fsPath.string()));
                files.push_back(info);
            }
            else if (entry.is_regular_file())
            {
                FileInfo info = GetFileInfos(Path(fsPath.string()));
                files.push_back(info);
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
        infos.name = path.Filename(false);
        infos.isDirectory = path.IsDirectory();
        infos.size = GetFileSize(path);
        infos.type = GetTypeFromExtensionString(GetExtension(path));

        return infos;
    }


    bool FileManager::Exists(const Path &path)
    {
        return std::filesystem::exists(path.full);
    }

    bool FileManager::IsDirectory(const Path &path)
    {
        return path.IsDirectory();
    }

    int FileManager::GetFileSize(const Path &path)
    {
        if (IsPackedResource(path)) {
            CosemUnPacker unpacker;
            unpacker.OpenArchive(path.GetParentArchive());
            auto fileIndex = unpacker.GetFileIndex();

            std::string insidePath = path.GetPathInsideArchive();

            if (IsDirectory(path) && !insidePath.empty() && insidePath.back() != '/')
                insidePath += '/';

            int totalSize = 0;

            for (const auto& entry : fileIndex) {
                if (entry.path == insidePath || entry.path.compare(0, insidePath.size(), insidePath) == 0) {
                    totalSize += entry.fileSize;
                }
            }

            return static_cast<int>(totalSize);
        } else {
            if (IsDirectory(path)) {
                int totalSize = 0;

                for (const auto& entry : std::filesystem::recursive_directory_iterator(path.full,
                            std::filesystem::directory_options::skip_permission_denied))
                {
                    if (entry.is_regular_file()) {
                        totalSize += std::filesystem::file_size(entry.path());
                    }
                }

                return static_cast<int>(totalSize);
            } else if (std::filesystem::exists(path.full) && std::filesystem::is_regular_file(path.full)) {
                return static_cast<int>(std::filesystem::file_size(path.full));
            } else {
                throw std::runtime_error("[ERROR]  [ENGINE/FILESYSTEM] : File or directory does not exist: " + path.full);
            }
        }
    }

    std::string FileManager::ReadFile(const Path &path)
    {
        if(IsPackedResource(path)){
            CosemUnPacker unpacker;
            unpacker.OpenArchive(path.GetParentArchive());
            std::vector<char> characters = unpacker.ExtractFileToMemory(path.GetPathInsideArchive());
            return std::string(characters.begin(), characters.end());
        }
        else{
            std::ifstream file(path.full, std::ios::binary);
            if (!file) throw std::runtime_error("[ERROR]  [ENGINE/FILESYSTEM] : Can't read file at path : " + path.full);
            std::ostringstream sstream;
            sstream << file.rdbuf();
            return sstream.str();
        }
    }

    bool FileManager::WriteFile(const Path &path, const std::string &content)
    {
        if (IsPackedResource(path)) {
            throw std::runtime_error("[ERROR]  [ENGINE/FILESYSTEM] : Cannot write to a file inside a .caf archive");
            return false;
        }
    
        std::ofstream out(path.full, std::ios::trunc); // truncate = overwrite if file exists
        if (!out.is_open()) return false;
    
        try {
            out << content;
        } catch (...) {
            return false;
        }
    
        return true;
    }

    bool FileManager::AppendToFile(const Path &path, const std::string &content)
    {
        if(IsPackedResource(path)){
            throw std::runtime_error("[ERROR]  [ENGINE/FILESYSTEM] : Cannot create append to file placed in .caf archive");
            return false;
        }

        std::ofstream out(path.full, std::ios::app);
        if (!out.is_open()) return false;
        try {
            out << content;
        } catch (...) {
            return false;
        }
        return true;
    }

    std::string FileManager::GetExtension(const Path &path)
    {
        return std::filesystem::path(path.GetAbsolutePath()).extension().string();
    }

    std::string FileManager::GetTypeString(const Type &type){
        switch(type){
            case Type::T_ANIMATION:
                return "ANIMATION_FILE_TYPE";
            case Type::T_CONFIG:
                return "CONFIG_FILE_TYPE";
            case Type::T_FONT:
                return "FONT_FILE_TYPE";
            case Type::T_IMAGE:
                return "IMAGE_FILE_TYPE";
            case Type::T_SCENE:
                return "SCENE_FILE_TYPE";
            case Type::T_SCRIPT:
                return "SCRIPT_FILE_TYPE";
            case Type::T_SHADER:
                return "SHADER_FILE_TYPE";
            case Type::T_TEXT:
                return "TEXT_FILE_TYPE";
            case Type::T_SOUND:
                return "SOUND_FILE_TYPE";
            case Type::T_DIRECTORY:
                return "DIRECTORY_FILE_TYPE";
            default:
                return "DEFAULT_FILE_TYPE";
        }
    }

    Type FileManager::GetTypeFromExtensionString(std::string str) {
        // Convert to lowercase for case-insensitive comparison
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
            {".glsl", Type::T_SHADER}, {".frag", Type::T_SHADER}, {".vert", Type::T_SHADER},
            // Text formats
            {".txt", Type::T_TEXT}, {".md", Type::T_TEXT},
            // Script formats
            {".lua", Type::T_SCRIPT}, {".py", Type::T_SCRIPT}, {".js", Type::T_SCRIPT},
            // Animation formats
            {".anim", Type::T_ANIMATION}, {".ani", Type::T_ANIMATION},
            // Scene formats
            {".scene", Type::T_SCENE}, {".scn", Type::T_SCENE},
            // Config formats
            {".json", Type::T_CONFIG}, {".ini", Type::T_CONFIG}, {".cfg", Type::T_CONFIG}, {".yaml", Type::T_CONFIG},
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

    std::string FileManager::GetFileName(const Path &path, bool withExtension)
    {
        std::filesystem::path fsPath = std::filesystem::path(path.GetAbsolutePath());

        if (withExtension)
            return fsPath.filename().string();
        else
            return fsPath.stem().string();
    }

    std::string FileManager::GetParentPath(const Path &path)
    {
        return std::filesystem::path(path.GetAbsolutePath()).parent_path().string();
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

    bool FileManager::IsPackedResource(const Path &path)
    {
        return path.full.find(".caf/") != std::string::npos;
    }

    Path::Path(const std::string &raw)
    {
        full = Normalize(raw);
    }

    std::string Path::Normalize(const std::string &raw)
    {
        return std::filesystem::weakly_canonical(raw).string();
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

    std::string Path::Filename(bool withExtension) const
    {
        auto p = std::filesystem::path(full);
        return withExtension ? p.filename().string() : p.stem().string();
    }

    std::string Path::Parent() const
    {
        return std::filesystem::path(full).parent_path().string();
    }

    std::string Path::GetParentArchive() const
    {
        if(!FileManager::IsPackedResource(*this)){
            throw std::runtime_error("[ERROR]  [ENGINE/FILESYSTEM] : Cannot get parent archive for a file that is not placed in an archive");
            return "";
        }
        else{

            size_t pos = full.find(".caf");
            if (pos == std::string::npos) {
                throw std::runtime_error("[ERROR]  [ENGINE/FILESYSTEM] : Packed resource path does not contain '.caf', it is not placed in an archive");
            }

            return full.substr(0, pos + 4);
        }
    }

    std::string Path::GetPathInsideArchive() const
    {
        const std::string &fullPath = this->full;
        size_t pos = fullPath.find(".caf");
        if (pos == std::string::npos) {
            throw std::runtime_error("[ERROR]   [ENGINE/FILESYSTEM] : File is not inside an archive");
        }

        size_t start = pos + 5;
        if (start >= fullPath.size()) return "";
    
        return fullPath.substr(start);
    }

    std::string Path::ResourcePath() const
    {
        std::size_t pos = full.find(".cosem");
        if (pos == std::string::npos) return full;
        return full.substr(0, pos);
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