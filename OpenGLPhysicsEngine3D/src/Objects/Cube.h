#pragma once

#include <glm/glm.hpp>

#include "Objects/Parts/Mesh.h"
#include "Interfaces/IEntity.h"

#include <iostream>
#include <vector>

class Cube : public IEntity
{
public:
	Cube();

	void Scale(glm::vec3 scale);
	void Rotate(glm::vec3 rotation);
	void Translate(glm::vec3 translation);

	AABB GetAABB();

	void Draw(const Shader& shader);

private:
	void InitializeMesh();
private:
	Mesh m_Mesh;
};

