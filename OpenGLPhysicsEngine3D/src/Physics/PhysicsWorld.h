#pragma once
#include <vector>

#include "Objects/Interfaces/IEntity.h"
#include "Interfaces/IPausable.h"

#include "Structures/CollisionPair.h"
#include "Structures/ContactPoint.h"

#include "Managers/EntityManager.h"

#include "OpenGL/Camera.h"

class PhysicsWorld : public IPausable
{
public:
	PhysicsWorld();

	Entity* SelectEntityWithScreenPosition(double xPos, double yPos, int windowWidth, int windowHeight, Camera* camera);

	void Update(float deltaTime, int iterations = 1);
	
	void ChangeState(ApplicationStates newState);

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

	void ResolveCollisionWithRotation3D(
		Entity& bodyA, Entity& bodyB,
		const glm::vec3& normal,
		float depth,
		const glm::vec3& contact);
private:
	bool m_Paused = false;
	static glm::vec3 m_Gravity;

	std::vector<CollisionPair> m_CollisionPairs;
	EntityManager m_Manager;
};

