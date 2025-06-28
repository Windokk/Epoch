#ifndef __COSEM_PACKER_H
#define __COSEM_PACKER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <algorithm>

#include <zlib.h>

class CosemPacker{

    public:
        void AddFile(const std::string &virtualPath, const uint8_t &flags = 0);
        void AddFile(const std::string &virtualPath, const std::vector<uint8_t> &fileData, const uint8_t &flags = 0, uint32_t originalSize = -1);
        bool Pack(const std::string &archivePath);

        void SetVerbose(bool activate) { verbose = activate; }

    private:
        bool verbose;

        struct FileEntry {
            std::string path;
            std::vector<uint8_t> data;
            uint8_t flags;
            uint32_t originalSize;
        };

        // FLAGS : 
        // 0x01 : Compress the file
        // 0x02 : TODO
        // 0x04 : TODO

        std::vector<FileEntry> files;

        bool CompressBuffer(const std::vector<uint8_t>& input, std::vector<uint8_t>& output);
};


#endif