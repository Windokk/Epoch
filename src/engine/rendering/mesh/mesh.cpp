#include "mesh.hpp"

#include <ufbx/ufbx.h>

#include "engine/rendering/renderer/renderer.hpp"

#include <iostream>

namespace SHAME::Engine::Rendering{
    
    Mesh::Mesh(const Filesystem::Path &path, COL_RGBA diffuse)
    {
        LoadMesh(path, diffuse);
    }

    Mesh::~Mesh()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    bool Mesh::LoadMesh(const Filesystem::Path &path, COL_RGBA diffuse)
    {
        ufbx_load_opts opts = { 0 }; // Optional, pass NULL for defaults
        ufbx_error error; // Optional, pass NULL if you don't care about errors
        const std::string filePath = path.full;
        ufbx_scene *scene = ufbx_load_file(filePath.c_str(), &opts, &error);
        if (!scene) {
            throw std::runtime_error(
                "[ERROR] [ENGINE/RENDERING/MESH] : Failed to load " + path.full + " : " +
                (error.description.data ? error.description.data : "Unknown error"));
            return false;
        }

        std::vector<uint32_t> indices;

        if (scene->meshes.count > 1) {
            ufbx_free_scene(scene);
            throw std::runtime_error("[ERROR] [ENGINE/RENDERING/MESH] : Engine doesn't support multiple meshes yet.");
        }

        ufbx_mesh* mesh = scene->meshes.data[0];


        for (size_t i = 0; i < mesh->num_faces; i++) {
            ufbx_face face = mesh->faces.data[i];

            size_t start = face.index_begin;
            size_t count = face.num_indices;

            // For each triangle in the fan: (0, j, j+1)
            for (size_t j = 1; j + 1 < count; j++) {
                Vertex verts[3];

                for (int k = 0; k < 3; k++) {
                    size_t vertex_index = start + (k == 0 ? 0 : j + k - 1);

                    Vertex vertex{};
                    ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, vertex_index);
                    vertex.position = { pos.x, pos.y, pos.z };

                    if (mesh->vertex_normal.exists) {
                        ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, vertex_index);
                        vertex.normal = { normal.x, normal.y, normal.z };
                    }

                    if (mesh->vertex_uv.exists) {
                        ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, vertex_index);
                        vertex.texCoord = { uv.x, uv.y };
                    }

                    vertex.color = diffuse;

                    // Push vertex and record index
                    verts[k] = vertex;
                }

                // Push the triangle vertices and indices (no deduplication)
                for (int k = 0; k < 3; ++k) {
                    vertices.push_back(verts[k]);
                    indices.push_back(static_cast<unsigned int>(vertices.size() - 1));
                }
            }
        }
        

        ufbx_free_scene(scene);

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