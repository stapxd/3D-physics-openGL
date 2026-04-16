#pragma once

#include <glm/glm.hpp>

struct LightData {
	glm::vec3 lightPosition = glm::vec3(15.0f, 15.0f, 15.0f);
	glm::vec3 lightColor = glm::vec3(1);
	glm::mat4 lightProjection = glm::mat4(1);
	glm::mat4 lightView = glm::mat4(1);

	float nearPlane = 1.0f;
	float farPlane = 50.0f;
	float orthoSize = 10.0f;
};