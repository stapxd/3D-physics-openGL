#pragma once

#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/Shader.h"

#include "Structures/Vertex.h"

class Mesh
{
public:
	Mesh(unsigned int vertexCount, unsigned int indexCount);
	Mesh() = delete;
	Mesh(const Mesh& copy) = delete;

	void Initialize(const VertexLayout& layout);

	void Scale(glm::vec3 scale);
	void Rotate(glm::vec3 rotation);
	void Translate(glm::vec3 translation);

	void Draw(const Shader& shader);

	void SetVertices(std::vector<Vertex> vertices);
	void SetIndices(std::vector<unsigned int> indices);

	const std::vector<glm::vec3>& GetTransformedVertices();

private:
	void UpdateTransformedVertices();
private:
	std::vector<Vertex> m_Vertices;
	std::vector<glm::vec3> m_TransformedVertices;
	std::vector<unsigned int> m_Indices;

	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<IndexBuffer> m_EBO;

	bool m_ShouldBeTransformed;

	glm::vec3 m_Scale;
	glm::vec3 m_Rotation;
	glm::vec3 m_Translation;
	
	// maybe without model
	glm::mat4 m_Model;
};

