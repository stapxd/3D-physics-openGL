#include "PhysicsWorld.h"

#include "Collisions.h"

#include <iostream>

void PhysicsWorld::Update(const std::vector<std::unique_ptr<IEntity>>& entities)
{
	for (int i = 0; i < entities.size(); i++) {
		IEntity* bodyA = entities[i].get();
		for (int j = i + 1; j < entities.size(); j++) {
			IEntity* bodyB = entities[j].get();

			ResolveAABBCollision(bodyA, bodyB);
		}
	}
}

void PhysicsWorld::ResolveAABBCollision(IEntity* bodyA, IEntity* bodyB)
{
	if (Collisions::CheckAABBCollision(bodyA, bodyB))
		std::cout << "Collision detected!\n";
	else
		std::cout << "No collision!\n";
}
