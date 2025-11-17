#pragma once

#include <glm/glm.hpp>

#include "Objects/Parts/Mesh.h"
#include "Objects/Parts/PhysicsBody.h"

#include "Interfaces/IEntity.h"

#include "Structures/OBB.h"

#include <iostream>
#include <vector>

class Cube : public IEntity
{
public:
	Cube();

	void Scale(glm::vec3 scale);
	void Rotate(glm::vec3 rotation);
	void Translate(glm::vec3 translation);

	void Move(glm::vec3 translation);

	// maybe not in here
	AABB GetAABB();
	OBB GetOBB();

	Mesh* GetMesh() { return &m_Mesh; }
	PhysicsBody* GetPhysicsBody() { return &m_PhysicsBody; }

	void Draw(const Shader& shader);

private:
	void InitializeMesh();
private:
	Mesh m_Mesh;
	PhysicsBody m_PhysicsBody;
};

