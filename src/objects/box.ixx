export module box;

import std;
import <glad/glad.h>;
import <glm/glm.hpp>;

import colors;
import lightedobject;
import math;

// Generates vertices with normals for a cube from (0, 0, 0) to (1, 1, 1)
std::vector<VertexWithNormal> createVerticesWithNormals()
{
    return {
        // Bottom face
        {{0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
        {{1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
        {{1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},

        // Top face
        {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},

        // Front face
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},

        // Back face
        {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},

        // Left face
        {{0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{0.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},

        // Right face
        {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}
    };
}

// Generates indices for rendering the cube using triangles
std::vector<unsigned int> createIndices()
{
    return {
        // Bottom face
        0, 1, 2, 0, 2, 3,
        // Top face
        4, 5, 6, 4, 6, 7,
        // Front face
        8, 9, 10, 8, 10, 11,
        // Back face
        12, 13, 14, 12, 14, 15,
        // Left face
        16, 17, 18, 16, 18, 19,
        // Right face
        20, 21, 22, 20, 22, 23
    };
}

export class Box : public LightedObject
{
public: 
    Box(float scale) : LightedObject(createVerticesWithNormals(), createIndices(), colors::transparentCyan)
    {
        model = math::scale({ scale, scale, scale }) * math::translate({ -0.5, -0.5, -0.5 });
    }
};