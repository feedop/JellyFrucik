export module handle;

import std;
import <glad/glad.h>;
import <glm/glm.hpp>;

import colors;
import lightedobject;
import glutils;

// Generates indices for rendering the cube using triangles
std::vector<unsigned int> createIndices()
{
    return {
        // Bottom face edges
        0, 1, 1, 2, 2, 3, 3, 0,
        // Top face edges
        4, 5, 5, 6, 6, 7, 7, 4,
        // Vertical edges connecting top and bottom faces
        0, 4, 1, 5, 2, 6, 3, 7
    };
}

export class Handle : public LightedObject
{
public:
    // Generates vertices for a cube from (0, 0, 0) to (1, 1, 1)
    static std::vector<glm::vec3> createVertices()
    {
        return {
            // Bottom face
            {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
            // Top face
            {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}
        };
    }

	Handle() : LightedObject(createVertices(), createIndices(), colors::green)
	{}

    virtual void draw(const Shader* shader) const override
    {
        ScopedBindArray ba(VAO);
        shader->setMatrix("model", model);
        shader->setVector("color", color);
        glDrawElements(GL_LINES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
    }
};