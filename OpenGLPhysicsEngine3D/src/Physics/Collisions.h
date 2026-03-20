#pragma once
#include "Objects/Entity.h"
#include "Structures/ContactPoint.h"
#include "Structures/Face.h"
#include "Structures/CollisionType.h"

#include <glm/glm.hpp>
#include <vector>
#include <limits>
#include <algorithm>

class Collisions
{
public:
    static bool CheckAABBCollision(Entity& bodyA, Entity& bodyB);
    static bool CheckOBBCollision(Entity& bodyA, Entity& bodyB, glm::vec3& normal, float& depth, std::vector<glm::vec3>& contactPoints);

    static bool CheckRayOBBCollision(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const OBB& obb, float& distance, glm::vec3* hitPoint);

    static std::vector<ContactPoint> GenerateOBBContactPoints(const OBB& A, const OBB& B, const glm::vec3& normal, float depth);

private:
    static std::vector<glm::vec3> GetOBBVertices(const OBB& obb);

    static Face GetIncidentFace(const OBB& obb, const glm::vec3& hitNormal);

    static void Clip(std::vector<glm::vec3>& points, const glm::vec3& planeNormal, float planeDist);
};
