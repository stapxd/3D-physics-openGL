#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Face.h"

struct CollisionManifold {
	std::shared_ptr<Face> reference;
	std::shared_ptr<Face> incident;

	glm::vec3 normal = glm::vec3(0.0f);
	float depth = 0.0f;

	std::vector<glm::vec3> pointsOfFaces;

	std::vector<glm::vec3> contactPoints;
};