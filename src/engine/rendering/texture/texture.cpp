#include "texture.hpp"

#include "engine/filesystem/filesystem.hpp"
#include "engine/rendering/renderer/renderer.hpp"

#include <iostream>

namespace SHAME::Engine::Rendering{

    using namespace Filesystem;

    Texture::Texture(Filesystem::Path filepath)
    {
        Init(filepath);
    }

    void Texture::Init(Filesystem::Path filepath)
    {
        infos.filepath = std::make_shared<Filesystem::Path>(filepath);

        // Load and set up the texture
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        unsigned char* data = nullptr;

        if (FileManager::Exists(*infos.filepath)) {
            std::string file = FileManager::ReadFile(*infos.filepath);
            
            stbi_set_flip_vertically_on_load(true);
            data = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(file.data()),
                                            static_cast<int>(file.size()),
                                            &infos.width, &infos.height, &infos.nrChannels, 0);

        } else {
            throw std::runtime_error(std::string("[ERROR]  [RENDERING/TEXTURE] Couldn't find texture : ") + infos.filepath->full);
            return;
        }

        if (data) {
            GLenum format;
            if (infos.nrChannels == 1)
                format = GL_RED;
            else if (infos.nrChannels == 3)
                format = GL_RGB;
            else if (infos.nrChannels == 4)
                format = GL_RGBA;
            else
                format = GL_RGB; // Default to RGB

            glTexImage2D(GL_TEXTURE_2D, 0, format, infos.width, infos.height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            throw std::runtime_error("[ERROR]  [RENDERING/TEXTURE] Couldn't load texture : " + filepath.full);
            return;
        }

        stbi_image_free(data);
    }

    void Texture::Bind(int unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    void Texture::UnBind(int unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Cleanup()
    {
        glDeleteTextures(1, &ID);

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        VAO = VBO = EBO = 0;
    }
}