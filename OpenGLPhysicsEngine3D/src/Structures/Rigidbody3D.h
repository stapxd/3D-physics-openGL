#pragma once

#include <glm/glm.hpp>

struct Rigidbody3D {
	bool isStatic;

	float mass;
	float restitution;

	glm::vec3 force = glm::vec3(0);
	glm::vec3 linearVelocity = glm::vec3(0);
};