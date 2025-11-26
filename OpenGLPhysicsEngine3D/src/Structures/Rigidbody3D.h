#pragma once

#include <glm/glm.hpp>

struct Rigidbody3D {
	bool isStatic;
	bool useGravity = true;

	float mass;
	float restitution;

	glm::vec3 force = glm::vec3(0);
	glm::vec3 linearVelocity = glm::vec3(0);
	glm::vec3 angularVelocity = glm::vec3(0);

	glm::mat3 inertiaTensor = glm::mat3(0);
	glm::mat3 inverseInertiaTensor = glm::mat3(0);
};