#pragma once
#include "Objects/Interfaces/IEntity.h"
class Collisions
{
public:
	static bool CheckAABBCollision(IEntity* bodyA, IEntity* bodyB);
};

