#pragma once
#include "Objects/Entity.h"
class Collisions
{
public:
	static bool CheckAABBCollision(Entity& bodyA, Entity& bodyB);
	static bool CheckOBBCollision (Entity& bodyA, Entity& bodyB, glm::vec3& normal, float& depth);

    static bool CheckRayOBBCollision(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const OBB& obb, float& distance);
};

