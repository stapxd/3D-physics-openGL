#pragma once
#include <vector>
#include <../Objects/Interfaces/IEntity.h>

#include "Managers/EntityManager.h"

#include "OpenGL/Camera.h"

class PhysicsWorld
{
public:
	PhysicsWorld();

	Entity* SelectEntityWithScreenPosition(double xPos, double yPos, int windowWidth, int windowHeight, Camera* camera);
	// temporary passing objects 
	void Update();
	
	EntityManager* operator->() { return &m_Manager; }
	std::unordered_map<unsigned int, Entity>& GetEntities() { return m_Manager.GetEntities(); }

protected:
	// TODO: add narrow and broad phase
	void SeparateBodies(Entity& bodyA, bool isStatic_A, Entity& bodyB, bool isStatic_B, glm::vec3 normal, float depth);

	// Collisions
	void ResolveAABBCollision(Entity& bodyA, Entity& bodyB);
	void ResolveOBBCollision(Entity& bodyA, Entity& bodyB);

private:
	EntityManager m_Manager;
};

