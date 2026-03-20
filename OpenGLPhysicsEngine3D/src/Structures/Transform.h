#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform {
	glm::vec3 translation = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1);
};