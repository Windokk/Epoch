#include "mesh.hpp"

#include "engine/rendering/renderer/renderer.hpp"

#include <iostream>

namespace EPOCH::Engine::Rendering{
    
    Mesh::Mesh(const ufbx_mesh* ufbx_mesh, double scene_unit_meters, ufbx_material_list& ufbx_mats, COL_RGBA diffuse)
    {
        LoadMesh(ufbx_mesh, scene_unit_meters, ufbx_mats, diffuse);
    }

    Mesh::~Mesh()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void ComputeTangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    {
        // Initialize tangent accumulators
        for (auto &v : vertices) {
            v.tangent = glm::vec3(0.0f);
        }

        // For each triangle:
        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            Vertex &v0 = vertices[indices[i + 0]];
            Vertex &v1 = vertices[indices[i + 1]];
            Vertex &v2 = vertices[indices[i + 2]];

            glm::vec3 edge1 = v1.position - v0.position;
            glm::vec3 edge2 = v2.position - v0.position;
            glm::vec2 deltaUV1 = v1.texCoord - v0.texCoord;
            glm::vec2 deltaUV2 = v2.texCoord - v0.texCoord;

            float denom = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            float f = (denom == 0.0f) ? 0.0f : 1.0f / denom;

            glm::vec3 tangent = glm::vec3(0.0f);
            tangent.x = f * ( deltaUV2.y * edge1.x - deltaUV1.y * edge2.x );
            tangent.y = f * ( deltaUV2.y * edge1.y - deltaUV1.y * edge2.y );
            tangent.z = f * ( deltaUV2.y * edge1.z - deltaUV1.y * edge2.z );

            // Accumulate to each vertex
            v0.tangent += tangent;
            v1.tangent += tangent;
            v2.tangent += tangent;
        }

        // Finally normalize and orthogonalize
        for (auto &v : vertices) {
            // Gram-Schmidt orthogonalization
            glm::vec3 n = v.normal;
            glm::vec3 t = v.tangent;

            // Remove component in normal direction:
            t = glm::normalize(t - n * glm::dot(n, t));

            v.tangent = t;
        }
    }

    bool Mesh::LoadMesh(const ufbx_mesh* ufbx_mesh, double scene_unit_meters, ufbx_material_list& ufbx_mats, COL_RGBA diffuse)
    {
        
        double scene_scale = scene_unit_meters;

        struct GroupedTriangles {
            std::vector<Vertex> verts;
            std::vector<unsigned int> localIndices;
            std::unordered_map<Vertex, unsigned int> vertexToIndex;
        };

        std::unordered_map<ufbx_material*, GroupedTriangles> materialGroups;
        std::vector<ufbx_material*> materialOrder;

        for (size_t i = 0; i < ufbx_mesh->num_faces; i++) {
            ufbx_face face = ufbx_mesh->faces.data[i];

            uint32_t mat_index = 0;
            if (ufbx_mesh->face_material.data && i < ufbx_mesh->face_material.count) {
                mat_index = ufbx_mesh->face_material.data[i];
            }

            ufbx_material* mat = nullptr;
            if (ufbx_mesh->materials.data && mat_index < ufbx_mesh->materials.count) {
                mat = ufbx_mesh->materials[mat_index];
            }

            // Fallback to global material list if needed
            if (!mat && ufbx_mats.count > 0 && ufbx_mats.data) {
                mat = ufbx_mats.data[0];
            }

            if (materialGroups.find(mat) == materialGroups.end()) {
                materialOrder.push_back(mat);
            }

            GroupedTriangles& group = materialGroups[mat];
            size_t start = face.index_begin;
            size_t count = face.num_indices;

            // Triangle fan triangulation
            for (size_t j = 1; j + 1 < count; j++) {
                size_t triIndices[3] = {
                    start + 0,
                    start + j,
                    start + j + 1
                };

                for (int k = 0; k < 3; k++) {
                    size_t vertex_index = triIndices[k];
                    Vertex v{};

                    // Position
                    ufbx_vec3 pos = ufbx_get_vertex_vec3(&ufbx_mesh->vertex_position, vertex_index);
                    v.position = { pos.x * scene_scale, pos.y * scene_scale, pos.z * scene_scale };

                    // Normal
                    if (ufbx_mesh->vertex_normal.exists) {
                        ufbx_vec3 normal = ufbx_get_vertex_vec3(&ufbx_mesh->vertex_normal, vertex_index);
                        v.normal = { normal.x, normal.y, normal.z };
                    }

                    // UV
                    if (ufbx_mesh->vertex_uv.exists) {
                        ufbx_vec2 uv = ufbx_get_vertex_vec2(&ufbx_mesh->vertex_uv, vertex_index);
                        v.texCoord = { uv.x, uv.y };
                    }

                    // Tangent
                    /*if (ufbx_mesh->vertex_tangent.exists) {
                        ufbx_vec3 tangent = ufbx_get_vertex_vec3(&ufbx_mesh->vertex_tangent, vertex_index);
                        v.tangent = { tangent.x, tangent.y, tangent.z };
                    }*/

                    // Color
                    v.color = diffuse;

                    // Deduplication
                    auto it = group.vertexToIndex.find(v);
                    if (it != group.vertexToIndex.end()) {
                        group.localIndices.push_back(it->second);
                    } else {
                        unsigned int newIndex = static_cast<unsigned int>(group.verts.size());
                        group.verts.push_back(v);
                        group.localIndices.push_back(newIndex);
                        group.vertexToIndex[v] = newIndex;
                    }
                }
            }
        }

        // Merge all material groups into one VBO/EBO
        vertices.clear();
        indices.clear();
        submeshes.clear();

        for (ufbx_material* mat : materialOrder) {
            auto& group = materialGroups[mat];
            size_t indexOffset = indices.size();
            size_t indexCount = group.localIndices.size();

            size_t vertexOffset = vertices.size();
            vertices.insert(vertices.end(), group.verts.begin(), group.verts.end());

            for (auto idx : group.localIndices)
                indices.push_back(static_cast<unsigned int>(vertexOffset + idx));

            submeshes.push_back(SubMesh{
                .indexOffset = indexOffset,
                .indexCount = indexCount
            });
        }

        totalIndexCount = indices.size();

        ComputeTangents(vertices, indices);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // layout: 0 - position, 1 - normal, 2 - color, 3 - texCoord, 4 - tangent 
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        glEnableVertexAttribArray(4);

        glBindVertexArray(0);

        return true;
    }
    
    void Mesh::DrawWithoutMaterial() const {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(totalIndexCount), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    std::vector<DrawCommand> Mesh::CreateDrawCmds(std::shared_ptr<ECS::Components::Transform> tr, int objectID, std::vector<std::shared_ptr<Material>> mats)
    {
        std::vector<DrawCommand> cmds;

        if(mats.size() != submeshes.size() && submeshes.size() != 1){
            DEBUG_ERROR("Cannot create draw command for meshes with different submeshes and materials count");
        }

        for (int i = 0; i < submeshes.size(); i++) {

            DrawCommand cmd;
            cmd.indexOffset = static_cast<int>(submeshes[i].indexOffset);
            cmd.indexCount  = static_cast<int>(submeshes[i].indexCount);
            cmd.VAO         = VAO;
            cmd.VBO         = VBO;
            cmd.mat         = mats[i];
            cmd.tr          = tr;
            cmd.id          = objectID;
            cmd.fillMode    = GL_FILL;

            cmds.push_back(std::move(cmd));
        }
        return cmds;
    }
}