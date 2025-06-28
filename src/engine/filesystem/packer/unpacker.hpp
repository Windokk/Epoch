#ifndef __COSEM_UNPACKER_H
#define __COSEM_UNPACKER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <algorithm>

#include <zlib.h>

class CosemUnPacker{
   

    public:
    
    struct FileEntry {
        uint16_t pathLength;
        std::string path;
        uint32_t dataOffset;
        uint32_t fileSize;
        uint32_t originalSize;
        uint8_t flags;
    };

    bool OpenArchive(const std::string& archivePath);
    bool ExtractFile(const std::string& filePath, const std::string& outputPath);
    std::vector<char> ExtractFileToMemory(const std::string& filePath);
    bool ExtractAll(const std::string& outputDir);
    void SetVerbose(bool activate) { verbose = activate; } 
    std::vector<FileEntry> GetFileIndex() const { return fileIndex; }

    private:

    bool verbose;


    // FLAGS : 
    // 0x01 : Uncompress the file
    // 0x02 : TODO
    // 0x04 : TODO

    struct ArchiveHeader {
        char magic[5];  // "COSEM" string
        uint8_t version;
        uint32_t fileCount;
        uint32_t indexOffset;
        uint32_t dataOffset;
    };

    std::ifstream archiveFile;
    ArchiveHeader header;
    std::vector<FileEntry> fileIndex;

    bool ReadHeader();
    bool ReadFileIndex();
    bool ExtractDataToFile(const FileEntry& file, const std::string& outputPath);
    std::vector<char> ExtractDataToMemory(const FileEntry& file);
    bool DecompressBuffer(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, uLong originalSize);
};

#endif