#pragma once

#include <glm/glm.hpp>

struct Simplex {
	glm::vec3 a = glm::vec3(0.0f);
	glm::vec3 b = glm::vec3(0.0f);
	glm::vec3 c = glm::vec3(0.0f);
	glm::vec3 d = glm::vec3(0.0f);

	unsigned int count = 0;
};