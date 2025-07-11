#include "font.hpp"

#include "engine/filesystem/filesystem.hpp"

namespace SHAME::Engine::Rendering::UI{

    using namespace Filesystem;
    
    Font::Font(const char *path, int size)
    {
        this->size = size;
    
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
        {
            throw std::runtime_error("[ERROR] [RENDERING/UI/FONT] Could not init FreeType Library");
            return;
        }
        
        // load font as face
        FT_Face face;
        FT_Error error;
        Path font_path = Path(path);

        if (font_path.Exists()) {
            std::string fontData = font_path.ReadFile();
    
            error = FT_New_Memory_Face(
                ft,                                              // FreeType library instance
                reinterpret_cast<const FT_Byte*>(fontData.data()), // Cast is necessary
                static_cast<FT_Long>(fontData.size()),                                    // Size of the font data
                0,                                               // Face index (for font collections, typically 0)
                &face                                            // Output face object
            );
    
        } else {
            throw std::runtime_error("[ERROR] [RENDERING/UI/FONT] Couldn't find font : " + std::string(path));
            return;
        }
    
        if (error) {
            throw std::runtime_error("[ERROR] [RENDERING/UI/FONT] Couldn't load font : " + std::string(path));
            return;
        }
        else {
            // set size to load glyphs as
            FT_Set_Char_Size(face, 0, size * 64, 300, 300);
    
            // disable byte-alignment restriction
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
            // load first 128 characters of ASCII set
            for (unsigned char c = 0; c < 128; c++)
            {
                // Load character glyph 
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    throw std::runtime_error("[ERROR] [RENDERING/UI/FONT] Couldn't load glyph");
                    continue;
                }
                // generate texture
                unsigned int texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
                );
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // now store character for later use
                Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    static_cast<unsigned int>(face->glyph->advance.x)
                };
                characters.insert(std::pair<char, Character>(c, character));
            }
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        // destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    
    
        // configure VAO/VBO for texture quads
        // -----------------------------------
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        // Position attribute (location = 0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::position));
        // Color attribute (location = 1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::color));
        // TexCoord attribute (location = 2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::texCoord));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Font::Cleanup(){
        for (auto& [c, character] : characters) {
            glDeleteTextures(1, &character.textureID);
        }
        characters.clear();
    
        // Supprimer les buffers OpenGL
        if (VBO) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
    
        if (EBO) {
            glDeleteBuffers(1, &EBO);
            EBO = 0;
        }
    
        if (VAO) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
    }
}