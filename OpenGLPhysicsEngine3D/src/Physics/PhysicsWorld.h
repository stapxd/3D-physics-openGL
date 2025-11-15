#pragma once
#include <vector>
#include <../Objects/Interfaces/IEntity.h>

#include "Managers/EntityManager.h"

class PhysicsWorld
{
public:
	PhysicsWorld();

	// temporary passing objects 
	void Update(const std::vector<std::unique_ptr<IEntity>>& entities);
	
	EntityManager* operator->() { return &m_Manager; }
	std::unordered_map<unsigned int, Entity>& GetEntities() { return m_Manager.GetEntities(); }

protected:
	// TODO: add narrow and broad phase
	void SeparateBodies(IEntity* bodyA, bool isStatic_A, IEntity* bodyB, bool isStatic_B, glm::vec3 normal, float depth);

	// Collisions
	void ResolveAABBCollision(IEntity* bodyA, IEntity* bodyB);
	void ResolveOBBCollision(IEntity* bodyA, IEntity* bodyB);

private:
	EntityManager m_Manager;
};

