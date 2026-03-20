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

	if (m_Properties.rigidbody.useGravity) {
		glm::vec3 acceleration(0.0f);

		if (!PhysicsWorld::GetIsVaccum()) {
			glm::vec3 gravityForce = m_Properties.rigidbody.mass * PhysicsWorld::GetGravity();

			glm::vec3 velocity = m_Properties.rigidbody.linearVelocity;
			float speed = glm::length(velocity);
			glm::vec3 dragForce(0.0f);

			if (speed > 0.0001f) {
				float rho = 1.225f;
				float Cd = 1.05f;

				float L = m_Properties.transform.scale.x * m_Properties.transform.scale.z;
				float A = 1.5f * (L * L);

				float dragMagnitude = 0.5f * rho * Cd * A * (speed * speed);
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
	else
		m_Properties.rigidbody.linearVelocity += m_Properties.rigidbody.force * deltaTime;

	Move(m_Properties.rigidbody.linearVelocity * deltaTime);

	m_Properties.rigidbody.force = glm::vec3(0);

	//AddRotation(m_Properties.rigidbody.angularVelocity * deltaTime);
	UpdateOrientation(deltaTime);
	m_Properties.rigidbody.angularVelocity *= glm::pow(0.99f, deltaTime);
	if (glm::length(m_Properties.rigidbody.angularVelocity) < 0.001f) {
		m_Properties.rigidbody.angularVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	m_Entity->ApplyTransform(m_Properties.transform);
}

void Entity::Move(glm::vec3 direction)
{
	m_Properties.transform.translation += direction;
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

