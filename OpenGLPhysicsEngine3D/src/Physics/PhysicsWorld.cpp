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

void PhysicsWorld::Update(const std::vector<std::unique_ptr<IEntity>>& entities)
{
	for (size_t i = 0; i < entities.size(); i++) {
		IEntity* bodyA = entities[i].get();
		for (size_t j = i + 1; j < entities.size(); j++) {
			IEntity* bodyB = entities[j].get();

			ResolveOBBCollision(bodyA, bodyB);
		}
	}
}

// TODO: check why bodies are not separeing!!!
void PhysicsWorld::SeparateBodies(IEntity* bodyA, bool isStatic_A, IEntity* bodyB, bool isStatic_B, glm::vec3 normal, float depth)
{
	if (isStatic_B) {
		bodyA->Move(normal * depth);
	}
	else if (isStatic_A) {
		bodyB->Move(-normal * depth);
	}
	else {
		bodyA->Move(normal * (depth / 2));
		bodyB->Move(-normal * (depth / 2));
	}
}

void PhysicsWorld::ResolveAABBCollision(IEntity* bodyA, IEntity* bodyB)
{
	if (Collisions::CheckAABBCollision(bodyA, bodyB))
		std::cout << "Collision detected!\n";
	else
		std::cout << "No collision!\n";
}

void PhysicsWorld::ResolveOBBCollision(IEntity* bodyA, IEntity* bodyB)
{
	glm::vec3 normal;
	float depth;

	bool isStatic_A = bodyA->GetPhysicsBody()->GetStatic();
	bool isStatic_B = bodyB->GetPhysicsBody()->GetStatic();

	if (Collisions::CheckOBBCollision(bodyA, bodyB, normal, depth)) {
		std::cout << "Collision detected!\n";
		
		SeparateBodies(bodyA, isStatic_A, bodyB, isStatic_B, normal, depth);
	}	
	else
		std::cout << "No collision!\n";
}
