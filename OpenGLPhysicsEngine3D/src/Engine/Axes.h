#pragma once

#include <glm/glm.hpp>

#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/Shader.h"

#include <iostream>
#include <vector>

class Axes
{
public:
	Axes();
	
	void Scale(glm::vec3 scale);
	
	void Draw(const Shader& shader);


private:
	void UpdateModel();
private:
	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;

	bool m_ModelShouldBeUpdated = true;

	/* Mesh.h */
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;

	glm::vec3 m_Scale;
	glm::mat4 m_Model;
};

