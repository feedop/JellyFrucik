export module lightedobject;

import std;

import <glad/glad.h>;
import <glm/mat4x4.hpp>;

import object;
import glutils;
import shader;

export struct VertexWithNormal
{
	glm::vec3 position;
	glm::vec3 normal;
};

export class LightedObject : public Object
{
public:
	virtual void draw(const Shader* shader) const
	{
		ScopedBindArray ba(VAO);
		shader->setMatrix("model", model);
		shader->setVector("color", color);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
	}

	void setData(const std::vector<glm::vec3>& vertices)
	{
		vertexCount = vertices.size();
		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_DYNAMIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	}

protected:
	size_t vertexCount;
	size_t indexCount;

	// Vertices + indices
	LightedObject(const std::vector<VertexWithNormal>& vertices, const std::vector<unsigned int>& indices, const glm::vec4& color) : Object(color), vertexCount(vertices.size()), indexCount(indices.size())
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexWithNormal), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWithNormal), (void*)0);
		// normal
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWithNormal), (void*)sizeof(glm::vec3));
	}

	// Vertices + indices, without normals
	LightedObject(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices, const glm::vec4& color) : Object(color), vertexCount(vertices.size()), indexCount(indices.size())
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		ScopedBindArray ba(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		// normal
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)sizeof(glm::vec3));
	}
};