#pragma once

#include "Objects/Entity.h"
#include <glm/glm.hpp>

struct ForceAtPoint {
	Entity& entity;
	glm::vec3 hitPoint;
	glm::vec3 forceDir;
	float forceMagnitude;

public:
	ForceAtPoint(Entity& _entity, glm::vec3 _hitPoint, glm::vec3 _forceDir, float _forceMagnitude)
		: entity(_entity), hitPoint(_hitPoint), forceDir(_forceDir), forceMagnitude(_forceMagnitude)
	{}
};