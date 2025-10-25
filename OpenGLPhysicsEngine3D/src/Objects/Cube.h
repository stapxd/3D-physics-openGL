#pragma once

#include <glm/glm.hpp>

#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/Shader.h"

#include <iostream>
#include <vector>

class Cube
{
public:
	Cube();

	void Scale(glm::vec3 scale);
	void Rotate(glm::vec3 rotation);
	void Translate(glm::vec3 translation);

	void Draw(const Shader& shader);

private:
	void UpdateModel();
private:
	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;
	// normals ???

	/* Mesh.h */
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<IndexBuffer> m_EBO;

	/* AKA IObject */
	bool m_ModelShouldBeUpdated = true;
	glm::vec3 m_Scale;
	glm::vec3 m_Rotation;
	glm::vec3 m_Translation;
	// maybe wwithout model
	glm::mat4 m_Model;
};

