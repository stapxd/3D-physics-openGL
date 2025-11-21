#pragma once
#include <vector>

#include "Objects/Interfaces/IEntity.h"

#include "Structures/CollisionPair.h"

#include "Managers/EntityManager.h"

#include "OpenGL/Camera.h"

class PhysicsWorld
{
public:
	PhysicsWorld();

	Entity* SelectEntityWithScreenPosition(double xPos, double yPos, int windowWidth, int windowHeight, Camera* camera);
	// temporary passing objects 
	void Update(float deltaTime, int iterations = 1);
	
	EntityManager* operator->() { return &m_Manager; }
	std::unordered_map<unsigned int, Entity>& GetEntities() { return m_Manager.GetEntities(); }
	
	static glm::vec3 GetGravity() { return m_Gravity; }

protected:
	void NarrowPhase();
	void BroadPhase();
	void MovementEntitiesStep(float deltaTime);

	void SeparateBodies(Entity& bodyA, bool isStatic_A, Entity& bodyB, bool isStatic_B, glm::vec3 normal, float depth);

	// Resolving
	void ResolveCollision(Entity& bodyA, Entity& bodyB, glm::vec3 normal, float depth);

private:
	static glm::vec3 m_Gravity;

	std::vector<CollisionPair> m_CollisionPairs;
	EntityManager m_Manager;
};

