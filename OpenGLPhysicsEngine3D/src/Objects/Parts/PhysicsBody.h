#pragma once

#include "Structures/Rigidbody3D.h"

class PhysicsBody
{
public:
	PhysicsBody();

	float GetInvertMass(const Rigidbody3D& rigidbody);
};

