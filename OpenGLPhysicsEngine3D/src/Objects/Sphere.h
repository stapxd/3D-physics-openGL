#pragma once

#include <glm/glm.hpp>

#include <iostream>
#include <vector>

#include "OpenGL/VertexArray.h"
#include "OpenGL/IndexBuffer.h"
#include "Structures/Vertex.h"


#include "Objects/Parts/Mesh.h"
#include "Objects/Parts/PhysicsBody.h"

#include "Interfaces/IEntity.h"

class Sphere : public IEntity
{
public:
	Sphere() = default;
	Sphere(float radius, int stackCount = 18, int sectorCount = 36);

	void ApplyTransform(const Transform& transform);

	AABB GetAABB();
	OBB GetOBB();

	void EstimateInertiaTensor(Rigidbody3D& rigidbody);

	Mesh* GetMesh() { return &m_Mesh; }
	PhysicsBody* GetPhysicsBody() { return &m_PhysicsBody; }
	
	// -------

	float GetRadius() const { return m_Radius; } //?
	int GetStackCount() const { return m_StackCount; } // ?
	int GetSectorCount() const { return m_SectorCount; } // ?

	void SetRadius(float radius);

	void Draw(const Shader& shader);

private:
	void BuildSphere(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
	void InitializeMesh();

private:
	
	float m_Radius = 1.0f;

	int m_StackCount = 18;
	int m_SectorCount = 36;

	Mesh m_Mesh;
	PhysicsBody m_PhysicsBody;
};

