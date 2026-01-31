#pragma once

#include <glm/glm.hpp>

struct Rigidbody3D {
	bool isStatic = false;
	bool useGravity = true;

	float mass = 1.0f;
	float restitution = 0.5f;

	glm::vec3 force = glm::vec3(0);
	glm::vec3 linearVelocity = glm::vec3(0);
	glm::vec3 angularVelocity = glm::vec3(0);

	glm::mat3 inertiaTensor = glm::mat3(0);
	glm::mat3 inverseInertiaTensorLocal = glm::mat3(0);
	glm::mat3 inverseInertiaTensorWorld = glm::mat3(0);
};