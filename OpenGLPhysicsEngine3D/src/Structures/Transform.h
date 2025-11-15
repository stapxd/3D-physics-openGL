#pragma once
#include <glm/glm.hpp>

struct Transform {
	glm::vec3 translation = glm::vec3(0); // position
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
};