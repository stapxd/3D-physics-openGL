#include "PhysicsWorld.h"

#include "Collisions.h"

#include <iostream>

#include "Enumerators/EntityTypes.h"

PhysicsWorld::PhysicsWorld()
{
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
