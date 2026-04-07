#pragma once

#include "Objects/Entity.h"

#include <glm/glm.hpp>
#include <vector>

#include "Structures/Simplex.h"
#include "Structures/EPAResult.h"

class EPA
{
public:
	static EPAResult Solve(const Simplex& simplex, Entity& bodyA, Entity& bodyB);

private:
	static std::pair<std::vector<glm::vec4>, size_t> GetFaceNormals(
		const std::vector<glm::vec3>& polytope,
		const std::vector<unsigned int>& faces);

	static void AddIfUniqueEdge(
		std::vector<std::pair<size_t, size_t>>& edges,
		const std::vector<size_t>& faces,
		size_t a,
		size_t b
	);
};

