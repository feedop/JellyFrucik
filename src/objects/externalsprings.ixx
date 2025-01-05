export module externalsprings;

import std;
import <glad/glad.h>;
import <glm/vec3.hpp>;
import <glm/gtc/quaternion.hpp>;
import <glm/gtx/quaternion.hpp>;

import colors;
import lightedobject;
import math;
import glutils;


std::vector<glm::vec3> createVertices()
{
	return std::vector<glm::vec3>(16, { 0, 0, 0 });
}

std::vector<unsigned int> createIndices()
{
	std::vector<unsigned int> indices;

	// Iterate through each face
	for (int i = 0; i < 16; i++)
	{
		indices.push_back(i);
	}

	return indices;
}

export class ExternalSprings : public LightedObject
{
public:
	ExternalSprings() : LightedObject(createVertices(), createIndices(), colors::orange)
	{}

	virtual void draw(const Shader* shader) const
	{
		ScopedBindArray ba(VAO);
		shader->setMatrix("model", model);
		shader->setVector("color", color);
		glDrawElements(GL_LINES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
	}
};