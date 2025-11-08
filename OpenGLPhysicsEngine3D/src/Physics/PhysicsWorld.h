#pragma once
#include <vector>
#include <../Objects/Interfaces/IEntity.h>

class PhysicsWorld
{
public:
	// temporary passing objects 
	void Update(const std::vector<std::unique_ptr<IEntity>>& entities);

	// TODO: add narrow and broad phase

	// Collisions
	void ResolveAABBCollision(IEntity* bodyA, IEntity* bodyB);
};

