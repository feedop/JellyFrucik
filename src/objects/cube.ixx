export module cube;

import std;
import <glad/glad.h>;
import <glm/vec3.hpp>;
import <glm/gtc/quaternion.hpp>;
import <glm/gtx/quaternion.hpp>;

import colors;
import lightedobject;
import math;
import glutils;

// 4 x 4 x 4 mass points
static consteval std::array<glm::vec3, 64> createBaseMassPointPositions()
{
	constexpr float dist = 1.0f / 3;
	std::array<glm::vec3, 64> positions;
	int counter = 0;
	for (int y = 0; y < 4; y++)
	{
		for (int z = 0; z < 4; z++)
		{
			for (int x = 0; x < 4; x++)
			{
				positions[counter++] = { x * dist, y * dist, z * dist };
			}
		}
	}
	return positions;
}

std::vector<unsigned int> createIndices()
{
	const int pointsPerFace = 16; // 4x4 grid on each face
	const int numFaces = 6;       // Cube has 6 faces
	const int gridSize = 4;       // Grid size per face
	std::vector<unsigned int> indices;

	// Iterate through each face
	for (int face = 0; face < numFaces; ++face)
	{
		int faceOffset = face * pointsPerFace; // Offset to the current face's starting point

		// Iterate through the 4x4 grid of points
		for (int row = 0; row < gridSize; ++row)
		{
			for (int col = 0; col < gridSize; ++col)
			{
				int currentIndex = faceOffset + row * gridSize + col;

				// Add horizontal line (if not on the last column)
				if (col < gridSize - 1)
				{
					indices.push_back(currentIndex);
					indices.push_back(currentIndex + 1);
				}

				// Add vertical line (if not on the last row)
				if (row < gridSize - 1)
				{
					indices.push_back(currentIndex);
					indices.push_back(currentIndex + gridSize);
				}
			}
		}
	}

	return indices;
}

export class Cube : public LightedObject
{
public:
	inline static constexpr std::array<glm::vec3, 64> baseMassPointPositions = createBaseMassPointPositions();

	// 6 x (4 x 4) points - each face to be rendered separately
	static std::vector<glm::vec3> createVertices(const std::array<glm::vec3, 64>& massPointPositions)
	{
		std::vector<glm::vec3> vertices;
		vertices.reserve(6 * 4 * 4 * sizeof(glm::vec3));

		// Bottom
		vertices.insert(vertices.end(), massPointPositions.begin(), massPointPositions.begin() + 16);

		// Top
		vertices.insert(vertices.end(), massPointPositions.end() - 16, massPointPositions.end());

		// Back
		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				vertices.push_back(massPointPositions[y * 16 + x]);
			}
		}

		// Front
		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				vertices.push_back(massPointPositions[y * 16 + 3 * 4 + x]);
			}
		}

		// Left
		for (int y = 0; y < 4; y++)
		{
			for (int z = 0; z < 4; z++)
			{
				vertices.push_back(massPointPositions[y * 16 + z * 4 + 0]);
			}
		}

		// Right
		for (int y = 0; y < 4; y++)
		{
			for (int z = 0; z < 4; z++)
			{
				vertices.push_back(massPointPositions[y * 16 + z * 4 + 3]);
			}
		}

		return vertices;
	}

	

	Cube() : LightedObject(createVertices(baseMassPointPositions), createIndices(), colors::red)
	{}

	virtual void draw(const Shader* shader) const
	{
		ScopedBindArray ba(VAO);
		glPatchParameteri(GL_PATCH_VERTICES, 16);
		shader->setVector("color", color);
		shader->setFloat("tessLevelOuter", 32.0f);
		shader->setFloat("tessLevelInner", 32.0f);
		for (int face = 0; face < 6; face++)
		{
			shader->setInt("face", face);
			glDrawArrays(GL_PATCHES, face * 16, 16);
		}
	}

	void drawWireframe(const Shader* shader) const
	{
		ScopedBindArray ba(VAO);
		shader->setMatrix("model", model);
		shader->setVector("color", colors::yellow);
		glDrawElements(GL_LINES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
		glPointSize(5.0);
		glDrawElements(GL_POINTS, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
	}
};