#include "Entity.h"

#include <glm/gtc/quaternion.hpp>

#include "Physics/PhysicsWorld.h"

Entity::Entity()
	: m_Id(-1)
{
}

Entity::Entity(unsigned int id, EntityTypes type, std::unique_ptr<IEntity> entity)
{
	m_Id = id;
	m_Type = type;
	m_Entity = std::move(entity);
}

void Entity::Step(float deltaTime)
{
	if (m_Properties.rigidbody.isStatic)
		return;

	glm::vec3 acceleration(0.0f);

	if (m_Properties.rigidbody.useGravity) {
		if (!PhysicsWorld::GetIsVaccum()) {
			glm::vec3 gravityForce = m_Properties.rigidbody.mass * PhysicsWorld::GetGravity();
			glm::vec3 velocity = m_Properties.rigidbody.linearVelocity;
			float speedSqr = glm::dot(velocity, velocity);
			glm::vec3 dragForce(0.0f);

			if (speedSqr > 0.0001f) {
				float speed = glm::sqrt(speedSqr);
				float rho = 1.225f;
				float Cd = 1.05f;
				float L = m_Properties.transform.scale.x * m_Properties.transform.scale.z;
				float A = 1.5f * (L * L);
				float dragMagnitude = 0.5f * rho * Cd * A * speedSqr;
				dragForce = -glm::normalize(velocity) * dragMagnitude;
			}

			glm::vec3 aggregateForce = gravityForce + dragForce + m_Properties.rigidbody.force;
			acceleration = aggregateForce / m_Properties.rigidbody.mass;
		}
		else {
			acceleration = PhysicsWorld::GetGravity();
		}
		m_Properties.rigidbody.linearVelocity += acceleration * deltaTime;
	}
	else {
		m_Properties.rigidbody.linearVelocity += (m_Properties.rigidbody.force / m_Properties.rigidbody.mass) * deltaTime;
	}

	Move(m_Properties.rigidbody.linearVelocity * deltaTime);
	UpdateOrientation(deltaTime);

	m_Properties.rigidbody.linearVelocity *= glm::pow(0.98f, deltaTime);
	m_Properties.rigidbody.angularVelocity *= glm::pow(0.95f, deltaTime);

	m_Properties.rigidbody.force = glm::vec3(0);
	m_Entity->ApplyTransform(m_Properties.transform);
}

void Entity::Move(glm::vec3 direction)
{
	m_Properties.transform.translation += direction;
}

void Entity::Wake()
{
	m_IsSleeping = false;
	m_SleepTimer = 0.0f;
}

void Entity::UpdateInertiaTensor()
{
	Rigidbody3D& rb = m_Properties.rigidbody;
	if (rb.isStatic)
		return;

	glm::mat3 R = glm::mat3_cast(m_Properties.transform.orientation);

	rb.inverseInertiaTensorWorld = R * rb.inverseInertiaTensorLocal * glm::transpose(R);
}

//void Entity::AddRotation(glm::vec3 rotation)
//{
//	m_Properties.transform.rotation += rotation;
//}

void Entity::UpdateOrientation(float deltaTime)
{
	glm::vec3 angularVel = m_Properties.rigidbody.angularVelocity;

	if (glm::length(angularVel) > 0.0001f) {
		glm::quat qW(0.0f, angularVel.x, angularVel.y, angularVel.z);

		m_Properties.transform.orientation += (qW * m_Properties.transform.orientation) * (deltaTime * 0.5f);
		m_Properties.transform.orientation = glm::normalize(m_Properties.transform.orientation);
	}
}

