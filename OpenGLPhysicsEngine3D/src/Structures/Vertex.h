#pragma once
#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normals;

	Vertex(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 normal = glm::vec3(0.0f))
		:position(pos), normals(normal)
	{}
};