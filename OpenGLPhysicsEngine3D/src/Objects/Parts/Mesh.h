#pragma once

#include <glm/gtc/quaternion.hpp>

#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/Shader.h"

#include "Structures/Vertex.h"
#include "Structures/Transform.h"

class Mesh
{
public:
	Mesh();
	Mesh(const Mesh& copy) = delete;

	void Initialize(std::vector<Vertex> vertices, std::vector<unsigned int> indices, const VertexLayout& layout);

	void Scale(glm::vec3 scale);
	void Rotate(glm::vec3 rotation);
	void SetOrientation(glm::quat orientation);
	void Translate(glm::vec3 translation);

	void SetTransform(const Transform& transform);
	// temporary in mesh -> physicsbody, but maybe leave here
	void Move(glm::vec3 translation);

	glm::mat4 GetModel();
	glm::vec3 GetScale() const { return m_Scale; }

	void Draw(const Shader& shader);

	const std::vector<Vertex>& GetTransformedVertices();

private:
	void UpdateTransformedVertices();
private:
	std::vector<Vertex> m_Vertices;
	std::vector<Vertex> m_TransformedVertices;
	std::vector<unsigned int> m_Indices;

	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<IndexBuffer> m_EBO;

	bool m_ShouldBeTransformed;
	bool m_Initialized = false;

	glm::vec3 m_Scale;
	glm::vec3 m_Rotation;		//depricated
	glm::quat m_Orientation;
	glm::vec3 m_Translation;
	
	glm::mat4 m_Model;
};

