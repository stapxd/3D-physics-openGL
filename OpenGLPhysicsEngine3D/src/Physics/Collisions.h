#pragma once
#include "Objects/Interfaces/IEntity.h"
class Collisions
{
public:
	static bool CheckAABBCollision(IEntity* bodyA, IEntity* bodyB);
	static bool CheckOBBCollision (IEntity* bodyA, IEntity* bodyB, glm::vec3& normal, float& depth);
};

