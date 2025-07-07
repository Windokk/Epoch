#pragma once

#include "engine/ecs/components/core/transform.hpp"
#include "engine/filesystem/filesystem.hpp"
#include "engine/rendering/material/material.hpp"

namespace SHAME::Engine::Rendering{

    class Mesh {
        public:
            Mesh(const Filesystem::Path& path, COL_RGBA diffuse = COL_RGBA(1,1,1,1));
            ~Mesh();

            void Draw() const;

            DrawCommand CreateDrawCmd();

        private:
            GLuint VAO, VBO, EBO;
            size_t indexCount;
            std::vector<Vertex> vertices;

            bool LoadMesh(const Filesystem::Path& path, COL_RGBA diffuse);
    };
    
}

