#pragma once
#include <vector>

#include "Objects/Interfaces/IEntity.h"
#include "Interfaces/IPausable.h"

#include "Structures/CollisionPair.h"
#include "Structures/ContactPoint.h"
#include "Structures/ForceAtPoint.h"

#include "Managers/EntityManager.h"

#include "OpenGL/Camera.h"


#include "Structures/Simplex.h"

class PhysicsWorld : public IPausable
{
public:
	PhysicsWorld();

	Entity* SelectEntityWithScreenPosition(double xPos, double yPos, int windowWidth, int windowHeight, Camera* camera, glm::vec3& rayDir, glm::vec3* hitPoint);
	void UpdateInertiaTensors();

	void Update(float deltaTime, int iterations = 1);
	void AddForceAtPoint(Entity& entity, glm::vec3 hitPoint, glm::vec3 forceDir, float forceMagnitude);
	
	void ChangeState(ApplicationStates newState);

	EntityManager* operator->() { return &m_Manager; }
	std::unordered_map<unsigned int, Entity>& GetEntities() { return m_Manager.GetEntities(); }
	
	static glm::vec3 GetGravity() { return m_Gravity; }
	static float GetDragCoeff() { return m_DragCoeff; }

	static void SetIsVaccum(bool v) { m_IsVacuum = v; }
	static bool GetIsVaccum() { return m_IsVacuum; }

#ifdef GJK_DEBUG

	const std::vector<glm::vec3>& GetMinkowsiDiff() const { return m_MinkowsiDiff; }
	std::vector<glm::vec3> GetSimplexPoints() const { 
		switch (m_Simplex.count)
		{
		case 1:
			return { m_Simplex.a};
		case 2:
			return { m_Simplex.a, m_Simplex.b};
		case 3:
			return { m_Simplex.a, m_Simplex.b, m_Simplex.c};
		case 4:
			return { m_Simplex.a, m_Simplex.b, m_Simplex.c, m_Simplex.d };
		default:
			return {};
		}
	}

	bool GetCollides() { return m_Collides; }

#endif


protected:
	void ApplyForceAtPoint();

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
		const std::vector<glm::vec3>& contactPoints);

	void ResolveCollisionWithRotationAndFriction3D(
		Entity& bodyA, Entity& bodyB,
		const glm::vec3& normal,
		float depth,
		const std::vector<glm::vec3>& contactPoints);
private:
	bool m_Paused = true;
	static glm::vec3 m_Gravity;
	static float m_DragCoeff;
	static bool m_IsVacuum;

	std::vector<ForceAtPoint> m_ForcesAtPoints;

	std::vector<CollisionPair> m_CollisionPairs;
	EntityManager m_Manager;




#ifdef GJK_DEBUG

	// GJK
	bool m_Collides = false;

	Simplex m_Simplex;

	std::vector<glm::vec3> m_MinkowsiDiff;
#endif
};

