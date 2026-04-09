#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Face.h"

struct CollisionManifold {

	std::shared_ptr<Face> reference;
	std::shared_ptr<Face> incident;
	std::vector<glm::vec3> pointsOfFaces;

	glm::vec3 normal = glm::vec3(0.0f);
	float depth = 0.0f;

	std::vector<glm::vec3> contactPoints;
};