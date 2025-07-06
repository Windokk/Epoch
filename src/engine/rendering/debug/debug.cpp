#include "debug.hpp"

namespace SHAME::Engine::Rendering{
    
    constexpr int LAT_SEGMENTS = 12;
    constexpr int LONG_SEGMENTS = 24;

    void SetupGLBuffers(GLuint &VAO, GLuint &VBO, GLuint &EBO, const std::vector<SimpleVertex> &vertices, const std::vector<GLuint> &indices) {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SimpleVertex), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)offsetof(SimpleVertex, position));

        glEnableVertexAttribArray(1); // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)offsetof(SimpleVertex, normal));

        glEnableVertexAttribArray(2); // color
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (void*)offsetof(SimpleVertex, color));

        glBindVertexArray(0);
    }

    DebugBox::DebugBox(glm::vec3 halfExtent, COL_RGBA color)
    {
        std::vector<glm::vec3> localCorners = {
            {-halfExtent.x, -halfExtent.y, -halfExtent.z},
            { halfExtent.x, -halfExtent.y, -halfExtent.z},
            { halfExtent.x,  halfExtent.y, -halfExtent.z},
            {-halfExtent.x,  halfExtent.y, -halfExtent.z},
            {-halfExtent.x, -halfExtent.y,  halfExtent.z},
            { halfExtent.x, -halfExtent.y,  halfExtent.z},
            { halfExtent.x,  halfExtent.y,  halfExtent.z},
            {-halfExtent.x,  halfExtent.y,  halfExtent.z}
        };

        vertices.resize(8);
        for (int i = 0; i < 8; ++i) {
            glm::vec3 worldPos = localCorners[i];
            vertices[i].position = worldPos;
            vertices[i].normal = glm::normalize(worldPos);
            vertices[i].color = color;
        }

        std::vector<GLuint> indices = {
            0,1, 1,2, 2,3, 3,0, // bottom square edges
            4,5, 5,6, 6,7, 7,4, // top square edges
            0,4, 1,5, 2,6, 3,7  // vertical edgess
        };

        SetupGLBuffers(VAO, VBO, EBO, vertices, indices);
        
        indexCount = indices.size();
    }

    DebugSphere::DebugSphere(float radius, COL_RGBA color)
    {
        std::vector<GLuint> indices;

        for (int lat = 0; lat <= LAT_SEGMENTS; ++lat) {
            float theta = glm::pi<float>() * lat / LAT_SEGMENTS;
            float sinT = sin(theta), cosT = cos(theta);

            for (int lon = 0; lon <= LONG_SEGMENTS; ++lon) {
                float phi = glm::two_pi<float>() * lon / LONG_SEGMENTS;
                float sinP = sin(phi), cosP = cos(phi);

                glm::vec3 normal = glm::vec3(cosP * sinT, cosT, sinP * sinT);
                glm::vec3 pos = radius * normal;

                vertices.push_back({ pos, normal, color });
            }
        }

        for (int lat = 0; lat < LAT_SEGMENTS; ++lat) {
            for (int lon = 0; lon < LONG_SEGMENTS; ++lon) {
                int cur = lat * (LONG_SEGMENTS + 1) + lon;
                int next = cur + LONG_SEGMENTS + 1;

                indices.push_back(cur);
                indices.push_back(next);
                indices.push_back(cur + 1);

                indices.push_back(next);
                indices.push_back(next + 1);
                indices.push_back(cur + 1);
            }
        }

        SetupGLBuffers(VAO, VBO, EBO, vertices, indices);

        indexCount = indices.size();
    }

    DebugCapsule::DebugCapsule(float radius, float halfHeight, COL_RGBA color)
    {
        constexpr int SEGMENTS = 12;
        constexpr int RINGS = 6;

        std::vector<GLuint> indices;

        // --- Precompute sin/cos tables for segments ---
        std::vector<float> sinTable(SEGMENTS + 1), cosTable(SEGMENTS + 1);
        for (int i = 0; i <= SEGMENTS; ++i) {
            float angle = glm::two_pi<float>() * i / SEGMENTS;
            sinTable[i] = sin(angle);
            cosTable[i] = cos(angle);
        }

        // === CYLINDER BODY ===
        for (int i = 0; i <= SEGMENTS; ++i) {
            glm::vec3 dir = glm::vec3(cosTable[i], 0.0f, sinTable[i]);

            vertices.push_back({ dir * radius + glm::vec3(0, -halfHeight, 0), glm::normalize(dir), color });
            vertices.push_back({ dir * radius + glm::vec3(0,  halfHeight, 0), glm::normalize(dir), color });
        }

        for (int i = 0; i < SEGMENTS; ++i) {
            int base = i * 2;
            indices.push_back(base);
            indices.push_back(base + 1);
            indices.push_back(base + 2);

            indices.push_back(base + 1);
            indices.push_back(base + 3);
            indices.push_back(base + 2);
        }

        auto addHemisphere = [&](float yOffset, bool flipY) {
            int startIndex = static_cast<int>(vertices.size());

            for (int ring = 0; ring <= RINGS; ++ring) {
                float v = (float)ring / RINGS * glm::half_pi<float>();
                float y = flipY ? -cos(v) : cos(v);
                float r = sin(v);

                for (int i = 0; i <= SEGMENTS; ++i) {
                    float x = cosTable[i];
                    float z = sinTable[i];

                    glm::vec3 normal = glm::vec3(x * r, y, z * r);
                    glm::vec3 pos = radius * normal + glm::vec3(0, yOffset, 0);
                    vertices.push_back({ pos, glm::normalize(normal), color });
                }
            }

            for (int ring = 0; ring < RINGS; ++ring) {
                for (int seg = 0; seg < SEGMENTS; ++seg) {
                    int a = startIndex + ring * (SEGMENTS + 1) + seg;
                    int b = a + SEGMENTS + 1;

                    indices.push_back(a);
                    indices.push_back(b);
                    indices.push_back(a + 1);

                    indices.push_back(b);
                    indices.push_back(b + 1);
                    indices.push_back(a + 1);
                }
            }
        };

        // === TOP HEMISPHERE ===
        addHemisphere(+halfHeight, false);

        // === BOTTOM HEMISPHERE ===
        addHemisphere(-halfHeight, true);

        SetupGLBuffers(VAO, VBO, EBO, vertices, indices);
        indexCount = indices.size();
    }

    DebugCylinder::DebugCylinder(float radius, float halfHeight, COL_RGBA color)
    {
        constexpr int SEGMENTS = 18; // minimal segment count for wireframe
        std::vector<GLuint> indices;

        // Vertex generation (top and bottom circles)
        for (int i = 0; i < SEGMENTS; ++i) {
            float angle = glm::two_pi<float>() * i / SEGMENTS;
            float x = cos(angle), z = sin(angle);
            glm::vec3 dir = glm::vec3(x, 0, z);

            // bottom ring
            vertices.push_back({ dir * radius + glm::vec3(0, -halfHeight, 0), dir, color});
            // top ring
            vertices.push_back({ dir * radius + glm::vec3(0, +halfHeight, 0), dir, color});
        }

        // Index generation
        for (int i = 0; i < SEGMENTS; ++i) {
            int next = (i + 1) % SEGMENTS;

            int b0 = i * 2;       // bottom current
            int t0 = b0 + 1;      // top current
            int b1 = next * 2;    // bottom next
            int t1 = b1 + 1;      // top next

            // Bottom circle
            indices.push_back(b0);
            indices.push_back(b1);

            // Top circle
            indices.push_back(t0);
            indices.push_back(t1);

            // Vertical lines
            indices.push_back(b0);
            indices.push_back(t0);
        }

        SetupGLBuffers(VAO, VBO, EBO, vertices, indices);
        indexCount = indices.size();
    }
    
    DebugShape::~DebugShape()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}