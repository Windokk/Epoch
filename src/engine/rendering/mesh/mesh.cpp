#include "mesh.hpp"

#include "tiny_obj_loader/tiny_obj_loader.h"

#include "engine/rendering/renderer/renderer.hpp"

#include <iostream>

namespace SHAME::Engine::Rendering{
    
    Mesh::Mesh(const Filesystem::Path &path)
    {
        LoadMesh(path);
    }

    Mesh::~Mesh()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    bool Mesh::LoadMesh(const Filesystem::Path &path)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.full.c_str());
        if (!warn.empty()) std::cout << "TinyObjLoader warning: " << warn << std::endl;
        if (!err.empty()) std::cerr << "TinyObjLoader error: " << err << std::endl;
        if (!ret) return false;

        std::vector<unsigned int> indices;

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};

                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                if (index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }

                if (index.texcoord_index >= 0) {
                    vertex.texCoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }

                vertex.color = glm::vec4(1,1,1,1);

                vertices.push_back(vertex);
                indices.push_back(static_cast<unsigned int>(indices.size()));
            }
        }

        indexCount = indices.size();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // layout: 0 - position, 1 - normal, 2 - texcoord
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);

        return true;
    }
    
    void Mesh::Draw() const {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    DrawCommand Mesh::CreateDrawCmd()
    {
        DrawCommand cmd{};

        cmd.VAO = VAO;
        cmd.indexCount = indexCount;
        cmd.VBO = VBO;
        cmd.vertices.insert(cmd.vertices.end(), std::begin(vertices), std::end(vertices));

        return cmd;
    }
}