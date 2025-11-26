#pragma once

#include "Structures/ObjectProperties.h"
#include "Structures/Rigidbody3D.h"

class IPhysicsBody {
public:
	virtual void EstimateInertiaTensor(Rigidbody3D& rigidbody) = 0;
};