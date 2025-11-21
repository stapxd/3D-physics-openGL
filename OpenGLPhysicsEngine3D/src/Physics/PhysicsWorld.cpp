#include "PhysicsWorld.h"
#include <iostream>

#include "Collisions.h"
#include "Enumerators/EntityTypes.h"

glm::vec3 PhysicsWorld::m_Gravity = glm::vec3(0.0f, -9.81f, 0.0f);

PhysicsWorld::PhysicsWorld()
{
}

Entity* PhysicsWorld::SelectEntityWithScreenPosition(double xPos, double yPos, int windowWidth, int windowHeight, Camera* camera)
{
	Entity* selected = nullptr;
	float minDist = FLT_MAX;
	float distance;

	float x = (2.0f * xPos) / windowWidth - 1.0f;
	float y = 1.0f - (2.0f * yPos) / windowHeight;
	float z = 1.0f;

	glm::vec3 rayNds = glm::vec3(x, y, z);

	glm::vec4 rayClip = glm::vec4(rayNds.x, rayNds.y, -1.0f, 1.0f);

	glm::mat4 proj = camera->GetProjection();
	glm::mat4 view = camera->GetView();

	glm::vec4 rayEye = glm::inverse(proj) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

	glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));

	for (auto& entity : m_Manager.GetEntities())
	{
		if (Collisions::CheckRayOBBCollision(camera->GetPosition(), rayWorld, entity.second->GetOBB(), distance))
		{
			if (distance < minDist) {
				minDist = distance;
				selected = &entity.second;
			}
		}
	}

	return selected;
}

void PhysicsWorld::Update(float deltaTime, int iterations)
{
	iterations = glm::clamp(iterations, 1, 128);

	float deltaTimePerIteration = deltaTime / iterations;

	for (int i = 0; i < iterations; i++) {

		MovementEntitiesStep(deltaTimePerIteration);
	}
	
	BroadPhase();
	NarrowPhase();
}

void PhysicsWorld::NarrowPhase()
{
	for (size_t i = 0; i < m_CollisionPairs.size(); i++) {
		Entity& bodyA = m_CollisionPairs[i].bodyA;
		Entity& bodyB = m_CollisionPairs[i].bodyB;

		glm::vec3 normal;
		float depth;

		bool isStatic_A = bodyA.GetProperties().rigidbody.isStatic;
		bool isStatic_B = bodyB.GetProperties().rigidbody.isStatic;

		if (Collisions::CheckOBBCollision(bodyA, bodyB, normal, depth)) {
			SeparateBodies(bodyA, isStatic_A, bodyB, isStatic_B, normal, depth);

			ResolveCollision(bodyA, bodyB, normal, depth);
		}
	}
}

void PhysicsWorld::BroadPhase()
{
	m_CollisionPairs.clear();

	for (unsigned int i = 0; i < m_Manager.GetSize(); i++) {
		Entity& bodyA = m_Manager.FindEntity(i);
		for (unsigned int j = i + 1; j < m_Manager.GetSize(); j++) {
			Entity& bodyB = m_Manager.FindEntity(j);

			bool isStatic_A = bodyA.GetProperties().rigidbody.isStatic;
			bool isStatic_B = bodyB.GetProperties().rigidbody.isStatic;

			if (isStatic_A && isStatic_B)
				continue;

			if (Collisions::CheckAABBCollision(bodyA, bodyB)) {
				m_CollisionPairs.push_back({ bodyA, bodyB });
			}
		}
	}
}

void PhysicsWorld::MovementEntitiesStep(float deltaTime)
{
	for (auto& entity : m_Manager.GetEntities()) {
		
		/*const ObjectProperties& properties = entity.second.GetProperties();
		entity.second->ApplyTransform(properties.transform);*/

		if (entity.second.GetProperties().rigidbody.isStatic) {
			const ObjectProperties& properties = entity.second.GetProperties();
			entity.second->ApplyTransform(properties.transform);
		}
		else {
			entity.second.Step(deltaTime);
		}
	}
}

// maybe add correction if it brokes
void PhysicsWorld::SeparateBodies(Entity& bodyA, bool isStatic_A, Entity& bodyB, bool isStatic_B, glm::vec3 normal, float depth)
{
	normal = glm::normalize(normal);

	if (isStatic_B) {
		bodyA.Move(-normal * depth);
	}
	else if (isStatic_A) {
		bodyB.Move(normal * depth);
	}
	else {
		bodyA.Move(-normal * (depth * 0.5f));
		bodyB.Move(normal * (depth * 0.5f));
	}
}

void PhysicsWorld::ResolveCollision(Entity& bodyA, Entity& bodyB, glm::vec3 normal, float depth)
{
	ObjectProperties& propertiesA = bodyA.GetProperties();
	ObjectProperties& propertiesB = bodyB.GetProperties();

	glm::vec3 relativeVelocity = propertiesA.rigidbody.linearVelocity - propertiesB.rigidbody.linearVelocity;

	float e = std::min(propertiesA.rigidbody.restitution, propertiesB.rigidbody.restitution);

	float numerator = -(1 + e) * glm::dot(relativeVelocity, normal);
	
	float massA = propertiesA.rigidbody.mass;
	float invMassA = (!propertiesA.rigidbody.isStatic && massA > 0.0f)
		? 1.0f / massA
		: 0.0f;

	float massB = propertiesB.rigidbody.mass;
	float invMassB = (!propertiesB.rigidbody.isStatic && massB > 0.0f)
		? 1.0f / massB
		: 0.0f;

	float denominator = invMassA + invMassB;
	float j = numerator / denominator;

	propertiesA.rigidbody.linearVelocity += j * invMassA * normal;
	propertiesB.rigidbody.linearVelocity -= j * invMassB * normal;
}
