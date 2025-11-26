#pragma once
#include "Objects/Entity.h"
#include "Structures/ContactPoint.h"
#include "Structures/CollisionType.h"

#include <glm/glm.hpp>
#include <vector>
#include <limits>
#include <algorithm>

class Collisions
{
public:
    static bool CheckAABBCollision(Entity& bodyA, Entity& bodyB);
    static bool CheckOBBCollision(Entity& bodyA, Entity& bodyB, glm::vec3& normal, float& depth, glm::vec3& contactPoint);

    static bool CheckRayOBBCollision(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const OBB& obb, float& distance);

    static std::vector<ContactPoint> GenerateOBBContactPoints(const OBB& A, const OBB& B, const glm::vec3& normal, float depth);
};
