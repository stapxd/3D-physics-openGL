#include "PhysicsBody.h"

PhysicsBody::PhysicsBody()
{
}

float PhysicsBody::GetInvertMass(const Rigidbody3D& rigidbody)
{
	if (rigidbody.isStatic)
		return 0.0f;

	if (rigidbody.mass == 0.0f) {
		return 0.0f;
	}

	return 1.0f / rigidbody.mass;
}
