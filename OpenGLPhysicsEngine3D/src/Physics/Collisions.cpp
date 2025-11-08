#include "Collisions.h"

bool Collisions::CheckAABBCollision(IEntity* bodyA, IEntity* bodyB)
{
    AABB a = bodyA->GetAABB();
    AABB b = bodyB->GetAABB();
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
        (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
        (a.min.z <= b.max.z && a.max.z >= b.min.z);
}
