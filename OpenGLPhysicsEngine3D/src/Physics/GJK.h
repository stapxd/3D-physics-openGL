#pragma once

#include "Objects/Entity.h"

#include <glm/glm.hpp>
#include <vector>

#include "Structures/Simplex.h"

class GJK
{
public:
	static bool Solve(Entity& bodyA, Entity& bodyB, Simplex& outSimplex, std::vector<glm::vec3>* minkowskiDifference);
	
	static std::vector<glm::vec3> GetOBBVertices(const OBB& obb);

	static glm::vec3 Support(std::vector<glm::vec3> vertices, glm::vec3 direction);
	static bool SameDirection(glm::vec3 v1, glm::vec3 v2);
private:

	static void DoSimplexLine(Simplex& simplex, glm::vec3& direction);
	static void DoSimplexTriangle(Simplex& simplex, glm::vec3& direction);
	static void DoSimplexTetrahedron(Simplex& simplex, glm::vec3& direction, bool& collide);

};

