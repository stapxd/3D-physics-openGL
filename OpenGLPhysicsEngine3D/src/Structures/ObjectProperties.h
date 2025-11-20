#pragma once
#include "Transform.h"
#include "Rigidbody3D.h"

struct ObjectProperties
{
	Transform transform;
	glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f);
	Rigidbody3D rigidbody;
};

