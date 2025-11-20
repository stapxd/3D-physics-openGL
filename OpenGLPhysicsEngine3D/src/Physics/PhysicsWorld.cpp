#include "PhysicsWorld.h"
#include <iostream>

#include "Collisions.h"
#include "Enumerators/EntityTypes.h"

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

void PhysicsWorld::Update()
{
	for (unsigned int i = 0; i < m_Manager.GetSize(); i++) {
		Entity& bodyA = m_Manager.FindEntity(i);
		for (unsigned int j = i + 1; j < m_Manager.GetSize(); j++) {
			Entity& bodyB = m_Manager.FindEntity(j);

			ResolveOBBCollision(bodyA, bodyB);
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


void PhysicsWorld::ResolveAABBCollision(Entity& bodyA, Entity& bodyB)
{
	if (Collisions::CheckAABBCollision(bodyA, bodyB))
		std::cout << "Collision detected!\n";
	else
		std::cout << "No collision!\n";
}

void PhysicsWorld::ResolveOBBCollision(Entity& bodyA, Entity& bodyB)
{
	glm::vec3 normal;
	float depth;

	bool isStatic_A = bodyA.GetProperties().physicsProperties.isStatic;
	bool isStatic_B = bodyB.GetProperties().physicsProperties.isStatic;

	if (Collisions::CheckOBBCollision(bodyA, bodyB, normal, depth)) {
		std::cout << "Collision detected!\n";
		
		SeparateBodies(bodyA, isStatic_A, bodyB, isStatic_B, normal, depth);
	}	
	else
		std::cout << "No collision!\n";
}
